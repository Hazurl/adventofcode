#include "../formatted_inputs/day_6.inc"

#include <vector>
#include <sstream>
#include <cmath>
#include <iostream>
#include <cassert>
#include <unordered_set>

struct Position {
    int x;
    int y;
};

struct Cell {
    int id;
    unsigned d;
};

namespace std
{
template <>
struct hash<Position>
{
    std::size_t operator()(const Position& c) const
    {
        return (c.x << 5) ^ c.y ^ (c.x >> 5);
    }
};
}

bool operator==(Position const& l, Position const& r) {
    return l.x == r.x && l.y == r.y;
}

Position parse(std::string const& s) {
    std::stringstream is(s);

    Position pos;

    is >> pos.x;
    assert(is.get() == ',');
    is >> pos.y;

    return pos;
}

int main() {
    int min_width{0};
    int max_width{0};

    int min_height{0};
    int max_height{0};

    std::array<Position, input.size()> positions;

    for(std::size_t i{0}; i < input.size(); ++i) {
        auto pos = parse(std::string{ input[i] });
        if (i == 0) {
            min_width = max_width = pos.x;
            min_height = max_height = pos.y;
        } else {
            min_width = std::min(min_width, pos.x);
            max_width = std::max(max_width, pos.x);
            min_height = std::min(min_height, pos.y);
            max_height = std::max(max_height, pos.y);
        }

        positions[i] = pos;
    }

    const std::size_t width = max_width - min_width + 1;
    const std::size_t height = max_height - min_height + 1;
    std::vector<std::vector<Cell>> map(width, std::vector<Cell>(height, { -1, 0 }));

    std::cout << "Bounds: " << width << ", " << height << '\n';
    std::cout << "Origin: " << min_width << ", " << min_height << '\n';

    for(std::size_t id{0}; id < positions.size(); ++id) {
        auto& p = positions[id];
        map[p.x - min_width][p.y - min_height] = { id, 0 };
    }

    auto at = [&] (int x, int y) -> Cell {
        if (x < 0 || x >= width || y < 0 || y >= height) return { -1, 0 };
        return map[x][y];
    };

    auto calcul = [&] (int x, int y) -> Cell {
        Cell current { -1, 0 };


        for(unsigned radius = 1; current.id == -1; ++radius) {
            for(Position p{x, y + static_cast<int>(radius)}; p.y > y; ++p.x, --p.y) {
                auto c = at(p.x, p.y);
                if (c.id >= 0 && c.d == 0) {
                    if (current.id == -1) {
                        current = { c.id, radius };
                    } else {
                        return { -2, radius };
                    }
                }
            }

            for(Position p{x + static_cast<int>(radius), y}; p.x > x; --p.x, --p.y) {
                auto c = at(p.x, p.y);
                if (c.id >= 0 && c.d == 0) {
                    if (current.id == -1) {
                        current = { c.id, radius };
                    } else {
                        return { -2, radius };
                    }
                }
            }

            for(Position p{x, y - static_cast<int>(radius)}; p.y < y; --p.x, ++p.y) {
                auto c = at(p.x, p.y);
                if (c.id >= 0 && c.d == 0) {
                    if (current.id == -1) {
                        current = { c.id, radius };
                    } else {
                        return { -2, radius };
                    }
                }
            }

            for(Position p{x - static_cast<int>(radius), y}; p.x < x; ++p.x, ++p.y) {
                auto c = at(p.x, p.y);
                if (c.id >= 0 && c.d == 0) {
                    if (current.id == -1) {
                        current = { c.id, radius };
                    } else {
                        return { -2, radius };
                    }
                }
            }
        }

        return current;
    };

    for(std::size_t x{0}; x < width; ++x) {
        for(std::size_t y{0}; y < height; ++y) {
            if (map[x][y].id == -1) {
                map[x][y] = calcul(x, y);
            }
        }
    }

    std::vector<bool> painted(width * height, false);

    bool is_border = false;
    long long max = 0;

    for(std::size_t _x{0}; _x < width; ++_x) {
        for(std::size_t _y{0}; _y < height; ++_y) {
            std::size_t i = _x * height + _y;

            if (!painted[i]) {
                std::unordered_set<Position> to_walk { {_x, _y} };
                auto id = map[_x][_y].id;
                is_border = false;
                long long size = 0;

                while(!to_walk.empty()) {
                    auto[x, y] = *to_walk.begin();
                    to_walk.erase(to_walk.begin());

                    is_border = is_border || x == 0 || x == width - 1 || y == 0 || y == height - 1;
                    ++size;
                    painted[x * height + y] = true;

                    {
                        Position up{ x, y - 1 };
                        if (up.x >= 0 && up.x < width && up.y >= 0 && up.y < height && !painted[up.x * height + up.y] && map[up.x][up.y].id == id) {
                            to_walk.insert(up);
                        }
                    }
                    {
                        Position down{ x, y + 1 };
                        if (down.x >= 0 && down.x < width && down.y >= 0 && down.y < height && !painted[down.x * height + down.y] && map[down.x][down.y].id == id) {
                            to_walk.insert(down);
                        }
                    }
                    {
                        Position left{ x - 1, y };
                        if (left.x >= 0 && left.x < width && left.y >= 0 && left.y < height && !painted[left.x * height + left.y] && map[left.x][left.y].id == id)  {
                            to_walk.insert(left);
                        }
                    }

                    {
                        Position right{ x + 1, y };
                        if (right.x >= 0 && right.x < width && right.y >= 0 && right.y < height && !painted[right.x * height + right.y] && map[right.x][right.y].id == id) {
                            to_walk.insert(right);
                        }
                    }
                }

                if (!is_border && max < size) max = size;
            }
        }
    }
    
    std::cout << ">>> " << max << '\n';

    long long zone{0};
    for(long x{0}; x < width; ++x) {
        for(long y{0}; y < height; ++y) {
            long long value{0};
            for(auto const& pos : positions) {
                value += std::abs(pos.x - x) + std::abs(pos.y - y);
            }

            if (value < 10000) zone++;
        }
    }

    std::cout << ">>> " << zone << '\n';

}