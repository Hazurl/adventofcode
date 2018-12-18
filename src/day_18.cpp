#include "../formatted_inputs/day_18.inc"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <set>
#include <iomanip>

static constexpr std::size_t width = input[0].size();
static constexpr std::size_t height = input.size();

enum class Acre {
    Open, Tree, Lumberyard
};

using Map = std::array<std::array<Acre, width>, height>;

struct Around {
    unsigned open{};
    unsigned tree{};
    unsigned lumberyard{};
};

Map parse_map() {
    Map map;

    for(std::size_t y{ 0 }; y < height; ++y) {
        for(std::size_t x{ 0 }; x < width; ++x) {
            switch(input[y][x]) {
                case '.': map[y][x] = Acre::Open; continue;
                case '|': map[y][x] = Acre::Tree; continue;
                case '#': map[y][x] = Acre::Lumberyard; continue;
                default: std::exit(2);
            }
        }
    }

    return map;
}

std::ostream& operator << (std::ostream& os, Map const& map) {
    for(std::size_t y{ 0 }; y < height; ++y) {
        for(std::size_t x{ 0 }; x < width; ++x) {
            switch(map[y][x]) {
                case Acre::Open: os << '.'; break;
                case Acre::Tree: os << '|'; break;
                case Acre::Lumberyard: os << '#'; break;
            }
        }
        os << '\n';
    }

    return os;
}

Around around(Map const& map, std::size_t x, std::size_t y) {
    Around ar;
    for(std::size_t dy = y == 0 ? 1 : 0; dy < (y == height - 1 ? 2 : 3); ++dy) {
        std::size_t y_coords = y + dy - 1;

        for(std::size_t dx = x == 0 ? 1 : 0; dx < (x == width - 1 ? 2 : 3); ++dx) {
            std::size_t x_coords = x + dx - 1;

            if (dx == 1 && dy == 1) continue;

            switch(map[y_coords][x_coords]) {
                case Acre::Open: ++ar.open; break;
                case Acre::Tree: ++ar.tree; break;
                case Acre::Lumberyard: ++ar.lumberyard; break;
            }
        }
    }

    return ar;
}

Acre next_state(Map const& map, std::size_t x, std::size_t y) {
    auto ar = around(map, x, y);
    switch(map[y][x]) {
        case Acre::Open: 
            if (ar.tree >= 3) {
                return Acre::Tree;
            } else {
                return Acre::Open;
            }

        case Acre::Tree:
            if (ar.lumberyard >= 3) {
                return Acre::Lumberyard;
            } else {
                return Acre::Tree;
            }

        case Acre::Lumberyard:
            if (ar.lumberyard >= 1 && ar.tree >= 1) {
                return Acre::Lumberyard;
            } else {
                return Acre::Open;
            }

        default: std::exit(1);
    }
}

Map iterate(Map const& last) {
    Map next;
    for(std::size_t x{ 0 }; x < width; ++x) {
        for(std::size_t y{ 0 }; y < height; ++y) {
            next[y][x] = next_state(last, x, y);
        }
    }

    return next;
}

long result(Map const& map) {
    long trees{};
    long lumberyards{};

    for(auto const& row : map) {
        trees += std::count(std::begin(row), std::end(row), Acre::Tree);
        lumberyards += std::count(std::begin(row), std::end(row), Acre::Lumberyard);
    }

    return trees * lumberyards;
}

struct hasher {
    std::size_t operator() (Map const& map) const {
        std::size_t h{};
        for(std::size_t x{ 0 }; x < width; ++x) {
            std::size_t n = static_cast<std::size_t>(map[0][x]);
            h ^= (h << 1) | n;
        }

        return h;
    }
};

std::size_t find_maximum_jump(std::set<std::size_t> const& previous, std::size_t current, std::size_t max) {
    for(auto p : previous) {
        std::size_t diff = current - p;
        if (current + diff < max) {
            std::size_t step = (max - current) / diff;
            return find_maximum_jump(previous, current + diff * step, max);
        }
    }

    return current;
}

long iterate_n(Map map, std::size_t iterations) {
    std::size_t percent = 0;
    std::unordered_map<Map, std::set<std::size_t>, hasher> known_maps;

    for(std::size_t it{ 0 }; it < iterations; ++it) {

        auto map_it = known_maps.find(map);
        if (map_it != std::end(known_maps)) {

            auto to = find_maximum_jump(map_it->second, it, iterations);
            std::cout << "Jump from " << it << " to " << to << '\n';
            map_it->second.insert(it);
            it = to;
            map_it->second.insert(it);

        } else {
            known_maps.emplace(map, std::set<std::size_t>{ it });
        }

        if ((it * 100) / iterations > percent) {
            percent = it * 100 / iterations;

            std::cout << "ITERATION: " << percent << "%" << '\n';
        }
        
        map = iterate(map);

    }

    return result(map);
}

long part_1() {
    return iterate_n(parse_map(), 10);
}

long part_2() {
    return iterate_n(parse_map(), 1'000'000'000);
}

int main() {
    std::cout << "Part 1: " << part_1() << '\n';
    std::cout << "Part 2: " << part_2() << '\n';
}

