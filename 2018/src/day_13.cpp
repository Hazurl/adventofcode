#include "../formatted_inputs/day_13.inc"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

enum class Track {
    Vertical, Horizontal, 
    DownLeft, DownRight,
    All
};

enum class Direction {
    Up, Down, Left, Right
};

enum class Schema {
    Straight,
    Left, Right
};

struct Cart {
    Direction dir;
    std::size_t x;
    std::size_t y;

    Schema schema{ Schema::Left };
};

Direction turn_left(Direction d) {
    switch(d) {
        case Direction::Left: return Direction::Down;
        case Direction::Down: return Direction::Right;
        case Direction::Right: return Direction::Up;
        case Direction::Up: return Direction::Left;
    }

    std::exit(3);
}

Direction turn_right(Direction d) {
    switch(d) {
        case Direction::Right: return Direction::Down;
        case Direction::Up: return Direction::Right;
        case Direction::Left: return Direction::Up;
        case Direction::Down: return Direction::Left;
    }

    std::exit(2);
}

int main() {

    std::array<std::array<Track, input[0].size() >, input.size()> tracks;
    std::vector<Cart> carts;
    auto hash_pair = [] (std::pair<std::size_t, std::size_t> const& p) { return p.first ^ p.second; };
    std::unordered_set<std::pair<std::size_t, std::size_t>, decltype(hash_pair)> positions(8, hash_pair);
    std::unordered_set<std::pair<std::size_t, std::size_t>, decltype(hash_pair)> crashed(8, hash_pair);

    {
        std::size_t x{ 0 };
        std::size_t y{ 0 };
        for(auto const& row : input) {
            for(char t : row) {
                switch(t) {
                    case '|': tracks[y][x] = Track::Vertical; break;
                    case '-': tracks[y][x] = Track::Horizontal; break;
                    case '/': tracks[y][x] = Track::DownRight; break;
                    case '\\': tracks[y][x] = Track::DownLeft; break;
                    case '+': tracks[y][x] = Track::All; break;

                    case '^': tracks[y][x] = Track::Vertical; carts.push_back({Direction::Up, x, y}); positions.emplace(x, y); break;
                    case 'v': tracks[y][x] = Track::Vertical; carts.push_back({Direction::Down, x, y}); positions.emplace(x, y); break;
                    case '<': tracks[y][x] = Track::Horizontal; carts.push_back({Direction::Left, x, y}); positions.emplace(x, y); break;
                    case '>': tracks[y][x] = Track::Horizontal; carts.push_back({Direction::Right, x, y}); positions.emplace(x, y); break;

                    case ' ': break;

                    default: std::cerr << "'" << t << "' (" << (int)t << ") ?\n"; return 1;
                }

                ++x;
            }

            ++y;
            x = 0;
        }
    }

    std::sort(std::begin(carts), std::end(carts), [] (Cart const& l, Cart const& r) {
        return l.y == r.y ? l.x < r.x : l.y < r.y;
    });

    std::size_t tick{ 0 };
    while(true) {
        std::cout << "TICK " << tick++ << '\n';

        for(auto p : positions) {
            std::cout << "# " << p.first << ", " << p.second << '\n';
        }

        for(auto& cart : carts) {

            if (crashed.count({cart.x, cart.y}) >= 1) {
                std::cout << "Pass " << cart.x << ", " << cart.y << '\n';
                continue;
            }

            positions.erase({cart.x, cart.y});

            switch(cart.dir) {
                case Direction::Up: --cart.y; break;
                case Direction::Down: ++cart.y; break;
                case Direction::Left: --cart.x; break;
                case Direction::Right: ++cart.x; break;
            }

            switch(tracks[cart.y][cart.x]) {
                case Track::Vertical: break;
                case Track::Horizontal: break;
                case Track::DownLeft: cart.dir = (cart.dir == Direction::Up || cart.dir == Direction::Down ? turn_left : turn_right)(cart.dir);break;
                case Track::DownRight: cart.dir = (cart.dir == Direction::Up || cart.dir == Direction::Down ? turn_right : turn_left)(cart.dir);break; break;
                case Track::All: { 
                    switch(cart.schema) {
                        case Schema::Left: cart.dir = turn_left(cart.dir); cart.schema = Schema::Straight; break; 
                        case Schema::Straight: cart.schema = Schema::Right; break; 
                        case Schema::Right: cart.dir = turn_right(cart.dir); cart.schema = Schema::Left; break; 
                    }
                    break;
                }
            }

            if (positions.count({cart.x, cart.y})) {
                std::cout << '\t' << cart.x << "," << cart.y << '\n';
                crashed.emplace(cart.x, cart.y);
                positions.erase({cart.x, cart.y});
                continue;
            }

            positions.emplace(cart.x, cart.y);
        }

        carts.erase(std::remove_if(std::begin(carts), std::end(carts), [&crashed] (Cart const& c) {
            return crashed.count({c.x, c.y}) >= 1;
        }), std::end(carts));

        crashed.clear();

        if (carts.size() <= 1) {
            for(auto const& c : carts) {
                std::cout << "> " << c.x << "," << c.y << '\n';
            }
            return 0;
        }

        std::sort(std::begin(carts), std::end(carts), [] (Cart const& l, Cart const& r) {
            return l.y == r.y ? l.x < r.x : l.y < r.y;
        });

    }
}