#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <deque>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <queue>

struct Doors {
    Doors(bool left = false, bool up = false) : left(left), up(up) {}
    bool left, up; 
};

struct Map {
    int top_left_x{ 0 };
    int top_left_y{ 0 };

    int width{ 0 };
    int height{ 0 };

    std::deque<std::deque<Doors>> doors;
};

void resize(Map& map, int x, int y, int width, int height) {
    //std::cout << "[" << map.top_left_x << " ~ " << map.width << " x " << map.top_left_y << " ~ " << map.height << "] => ";
    //std::cout << "[" << x << " ~ " << width << " x " << y << " ~ " << height << "]\n";    
    
    for(auto& row : map.doors) {
        for(int i{ map.top_left_x + map.width }; i < x + width; ++i) {
            //std::cout << " > push back column\n";
            row.push_back(false);
        }

        for(int i{ x }; i < map.top_left_x; ++i) {
            //std::cout << " > push front column\n";
            row.push_front(false);
        }
    }

    for(int i{ map.top_left_y + map.height }; i < y + height; ++i) {
        //std::cout << " > push back row\n";
        map.doors.push_back(std::deque<Doors>(width, Doors{}));
    }

    for(int i{ y }; i < map.top_left_y; ++i) {
        //std::cout << " > push front row\n";
        map.doors.push_front(std::deque<Doors>(width, Doors{}));
    }

    assert(map.top_left_x >= x);
    assert(map.top_left_y >= y);
    assert(map.width <= width);
    assert(map.height <= height);

    map.top_left_x = x;
    map.top_left_y = y;
    map.width = width;
    map.height = height;
}

Doors& get_doors(Map& map, int x, int y) {
    //std::cout << "# " << x << ", " << y;
    auto cx = x - map.top_left_x;
    auto cy = y - map.top_left_y;
    //std::cout << " (" << cx << ", " << cy << ")\n";

    if (cx < 0 || cx >= map.width || cy < 0 || cy >= map.height) {
        resize(map, 
            std::min(cx + map.top_left_x, map.top_left_x),
            std::min(cy + map.top_left_y, map.top_left_y),
            std::max(cx < 0 ? (map.width - cx) : (cx + 1), map.width),
            std::max(cy < 0 ? (map.height - cy) : (cy + 1), map.height));

        cx = x - map.top_left_x;
        cy = y - map.top_left_y;
    }
    //std::cout << "(" << cx << ", " << cy << ")\n";

    return map.doors.at(cy).at(cx);
}

void set_west_door(Map& map, int x, int y) {
    get_doors(map, x, y).left = true;
}

void set_north_door(Map& map, int x, int y) {
    get_doors(map, x, y).up = true;
}

int ident = 0;

struct Hasher {
    std::size_t operator()(std::pair<int, int> const& p) const {
        return (p.first << 5) ^ p.second ^ (p.first >> 7);
    }
};

using positions_set_t = std::unordered_set<std::pair<int, int>, Hasher>;

positions_set_t create_map(Map& map, int sx, int sy, std::istream& input);

positions_set_t create_map_group(Map& map, positions_set_t const& sp, std::istream& input) {
    positions_set_t positions;

    while(true) {
        switch(input.peek()) {
            case '(':
            case '|': {
                input.get();
                auto old = input.tellg();
                for(auto const& p : sp) {
                    input.seekg(old);
                    std::cout << std::string(ident, '\t') << "> New child at " << old << " from (" << p.first << ", " << p.second << ")\n";

                    ++ident;

                    auto childs = create_map(map, p.first, p.second, input);
                    positions.merge(std::move(childs));

                    --ident;

                }
                break;
            }

            case ')':
                input.get();
                return positions;

            case '$':
                return positions;
            
            default: std::cerr << "create_map_group :: error '" << input.peek() << "'\n"; std::exit(1);
        }
    }
}

positions_set_t create_map(Map& map, int sx, int sy, std::istream& input) {
    positions_set_t positions{{ sx, sy }};

    while(true) {
        switch(input.peek()) {
            case '(': {
                std::cout << std::string(ident, '\t') << "> New Group at " << input.tellg() << '\n';
                ++ident;
                positions = create_map_group(map, positions, input);
                --ident;
                break;
            }

            case '|':
            case '$':
            case ')':
                return positions;

            case 'N': {
                input.get();

                positions_set_t next;

                for(auto& p : positions) {
                    std::cout << std::string(ident, '\t') << "> Move North at " << input.tellg() << " from (" << p.first << ", " << p.second << ")\n";
                    ++ident;
                    set_north_door(map, p.first, p.second);
                    --ident;
                    next.emplace(p.first, p.second - 1);
                }

                positions = std::move(next);
                break;
            }

            case 'W': {
                input.get();

                positions_set_t next;

                for(auto& p : positions) {
                    std::cout << std::string(ident, '\t') << "> Move West at " << input.tellg() << " from (" << p.first << ", " << p.second << ")\n";
                    ++ident;
                    set_west_door(map, p.first, p.second);
                    --ident;
                    next.emplace(p.first - 1, p.second);
                }

                positions = std::move(next);
                break;
            }
            
            case 'S': {
                input.get();

                positions_set_t next;

                for(auto& p : positions) {
                    std::cout << std::string(ident, '\t') << "> Move South at " << input.tellg() << " from (" << p.first << ", " << p.second << ")\n";
                    ++ident;
                    set_north_door(map, p.first, p.second + 1);
                    next.emplace(p.first, p.second + 1);
                    --ident;
                }

                positions = std::move(next);
                break;
            }
            
            case 'E': {
                input.get();

                positions_set_t next;

                for(auto& p : positions) {
                    std::cout << std::string(ident, '\t') << "> Move East at " << input.tellg() << " from (" << p.first << ", " << p.second << ")\n";
                    ++ident;
                    set_west_door(map, p.first + 1, p.second);
                    next.emplace(p.first + 1, p.second);
                    --ident;
                }

                positions = std::move(next);
                break;
            }
            
            default: std::cerr << "create_map :: error '" << input.peek() << "'\n"; std::exit(1);
        }
    }
}

Map create_map(std::istream& input) {
    Map map;

    assert(input.get() == '^');
    create_map(map, 0, 0, input);
    assert(input.get() == '$');

    return map;
}

using DistanceMap = std::vector<std::vector<int>>;


struct DistancePosition {
    int distance;
    int x;
    int y;
};

bool operator <(DistancePosition const& l, DistancePosition const& r) {
    return  l.distance < r.distance;
}

bool operator >(DistancePosition const& l, DistancePosition const& r) {
    return  l.distance > r.distance;
}

DistancePosition get_max_position(Map const& map) {
    std::priority_queue<DistancePosition, std::vector<DistancePosition>, std::greater<DistancePosition>> positions;
    DistanceMap distances(map.height + 1, std::vector<int>(map.width + 1, std::numeric_limits<int>::max()));

    DistancePosition max = {0, -map.top_left_x, -map.top_left_y};

    positions.push(max);

    std::cout << "[" << map.top_left_x << " ~ " << map.width << " x " << map.top_left_y << " ~ " << map.height << "]\n";

    auto door_at = [&map] (int x, int y) {
        if (x < 0 || x >= map.width || y < 0 || y >= map.height) {
            std::cout << "(OOB: " << x << ", " << y << ")\n";
            return Doors(false, false);
        }
        return map.doors[y][x];
    };

    auto dist_at = [&distances] (int x, int y) -> int& {
        return distances.at(y + 1).at(x + 1);
    };

    while(!positions.empty()) {
        auto cur = positions.top();
        positions.pop();

        std::cout << "Working on (" << cur.x << ", " << cur.y << "): " << cur.distance << '\n'; 
/*
        if (dist_at(cur.x, cur.y) != std::numeric_limits<int>::max()) {
            continue;
        }
*/
        if (door_at(cur.x, cur.y).up){
            std::cout << "\tUp ?";
            auto up = cur;
            --up.y;
            ++up.distance;

            if (dist_at(up.x, up.y) > up.distance) {
                dist_at(up.x, up.y) = up.distance;

                positions.push(up);

                if (max < up) max = up;

                std::cout << " ok\n";
            } else {
                std::cout << '\n';
            }
        }

        if (door_at(cur.x, cur.y + 1).up){
            std::cout << "\tDown ?";
            auto down = cur;
            ++down.y;
            ++down.distance;

            if (dist_at(down.x, down.y) > down.distance) {
                dist_at(down.x, down.y) = down.distance;

                positions.push(down);

                if (max < down) max = down;

                std::cout << " ok\n";
            } else {
                std::cout << '\n';
            }
        }

        if (door_at(cur.x, cur.y).left){
            std::cout << "\tLeft ?";
            auto left = cur;
            --left.x;
            ++left.distance;

            if (dist_at(left.x, left.y) > left.distance) {
                dist_at(left.x, left.y) = left.distance;

                positions.push(left);

                if (max < left) max = left;

                std::cout << " ok\n";
            } else {
                std::cout << '\n';
            }
        }

        if (door_at(cur.x + 1, cur.y).left){
            std::cout << "\tRight ?";
            auto right = cur;
            ++right.x;
            ++right.distance;

            if (dist_at(right.x, right.y) > right.distance) {
                dist_at(right.x, right.y) = right.distance;

                positions.push(right);

                if (max < right) max = right;

                std::cout << " ok\n";
            } else {
                std::cout << '\n';
            }
        }
    }

    std::cout << "Max: " << (max.x += map.top_left_x) << ", " << (max.y += map.top_left_y) << " with " << max.distance << '\n';

    int total{ 0 };
    for(auto const row : distances) {
        total += std::count_if(std::begin(row), std::end(row), [] (auto d) { return d != std::numeric_limits<int>::max() && d >= 1000; });
    }

    std::cout << "At least 1000 doors: " << total << '\n';

    return max;
}

int main() {

    std::ifstream file("formatted_inputs/day_20");
    if (!file) {
        std::cerr << "Oops, can't open file...\n";
        return 1;
    }

    auto map = create_map(file);

    std::cout << std::string(map.width * 2 + 3, '#') << '\n';
    for(int x{ 0 }; x <= map.width; ++x) {
        std::cout << "#.";
    }
    std::cout << "#\n";

    for(int y{ 0 }; y < map.height; ++y) {
        std::cout << "###";
        for(int x{ 0 }; x < map.width; ++x) {
            std::cout << (map.doors[y][x].up ? '-' : '#') << "#";
        }
        std::cout << "\n";

        std::cout << "#";
        for(int x{ 0 }; x < map.width; ++x) {
            std::cout << '.' << (map.doors[y][x].left ? '|' : '#');
        }
        std::cout << ".#\n";
    }
    std::cout << std::string(map.width * 2 + 3, '#') << '\n';

    auto max = get_max_position(map);

}