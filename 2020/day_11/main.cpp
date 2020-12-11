#include "../common.hpp"

enum class Cell {
    Floor, Empty, Occupied
};

struct Map {
    std::vector<Cell> cells;
    std::size_t width;
    std::size_t height;

    Map(std::size_t width_, std::size_t height_) : cells(width_ * height_), width(width_), height(height_) {}

    Cell& operator[](std::pair<std::size_t, std::size_t> const& xy) {
        return cells[xy.first + xy.second * width];
    }

    Cell const& operator[](std::pair<std::size_t, std::size_t> const& xy) const {
        return cells[xy.first + xy.second * width];
    }
};

Cell char_to_cell(char c) {
    switch(c) {
        case 'L': return Cell::Empty;
        case '#': return Cell::Occupied;
        case '.': return Cell::Floor;

        default: throw std::runtime_error("Error parsing cell from char '" + std::string(1, c) + "'");
    }
}

char cell_to_char(Cell c) {
    switch(c) {
        case Cell::Empty: return 'L';
        case Cell::Occupied: return '#';
        case Cell::Floor: return '.';

        default: throw std::runtime_error("Error parsing cell to char");
    }
}

void print_map(Map const& map) {
    for(std::size_t y = 0; y < map.height; ++y) {
        for(std::size_t x = 0; x < map.width; ++x) {
            std::cout << cell_to_char(map[{x, y}]);
        }
        std::cout << '\n';
    }
}

Map read_map(std::vector<std::string> const& lines) {
    auto const width = lines.front().size();
    auto const height = lines.size();
    Map map(width, height);

    for(std::size_t x = 0; x < width; ++x) {
        for(std::size_t y = 0; y < height; ++y) {
            map[{x, y}] = char_to_cell(lines[y][x]);
        }
    }

    return map;
}

std::size_t count_occupied(Map const& map, std::size_t x, std::size_t y) {
    std::size_t count = 0;

    for(long dx = -1; dx <= 1; ++dx) {
        if (x == 0 && dx < 0) continue;
        if (x == map.width - 1 && dx > 0) continue;

        for(long dy = -1; dy <= 1; ++dy) {
            if (y == 0 && dy < 0) continue;
            if (y == map.height - 1 && dy > 0) continue;

            if (dx == 0 && dy == 0) continue;

            std::size_t const cx = x + dx;
            std::size_t const cy = y + dy;
            auto const cell = map[{ cx, cy }];

            if (Cell::Occupied == cell) {
                ++count;
            }
        }
    }

    return count;
}

bool is_seeing_occupied_cell(Map const& map, std::size_t x, std::size_t y, long dx, long dy) {
    long cx = x + dx;
    long cy = y + dy;

    for(
        long cx = x + dx, 
        cy = y + dy; 
        cx >= 0 && cx < map.width && cy >= 0 && cy < map.height; 
        cx += dx, 
        cy += dy
    ) {
        if (map[{ cx, cy }] == Cell::Occupied) return true;
        if (map[{ cx, cy }] == Cell::Empty) return false;
    }

    return false;
}

std::size_t count_occupied_variant(Map const& map, std::size_t x, std::size_t y) {
    std::size_t count = 0;

    for(long dx = -1; dx <= 1; ++dx) {
        for(long dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;

            auto is_occupied = is_seeing_occupied_cell(map, x, y, dx, dy);
            if (is_occupied) {
                ++count;
            }
        }
    }

    return count;
}

Cell next_step_of(Map const& map, std::size_t x, std::size_t y) {
    auto count = count_occupied(map, x, y);

    auto current = map[{ x, y }];

    switch(current) {
        case Cell::Empty: 
            return count == 0 ? Cell::Occupied : Cell::Empty; 

        case Cell::Occupied: 
            return count >= 4 ? Cell::Empty : Cell::Occupied; 

        case Cell::Floor: 
            return Cell::Floor; 
    }

    throw std::runtime_error("???");
}

Cell next_step_of_variant(Map const& map, std::size_t x, std::size_t y) {
    auto count = count_occupied_variant(map, x, y);

    auto current = map[{ x, y }];

    switch(current) {
        case Cell::Empty: 
            return count == 0 ? Cell::Occupied : Cell::Empty; 

        case Cell::Occupied: 
            return count >= 5 ? Cell::Empty : Cell::Occupied; 

        case Cell::Floor: 
            return Cell::Floor; 
    }

    throw std::runtime_error("???");
}

std::pair<Map, std::size_t> step(Map const& map, Cell (*step_cell)(Map const&, std::size_t, std::size_t)) {
    Map next_map(map.width, map.height);
    std::size_t difference = 0;

    for(std::size_t x = 0; x < map.width; ++x) {
        for(std::size_t y = 0; y < map.height; ++y) {
            auto current = map[{ x, y }];
            auto& next_cell = next_map[{ x, y }] = step_cell(map, x, y);

            if (current != next_cell) {
                ++difference;
            }
        }
    }

    return { std::move(next_map), difference };
} 

long part_1(std::vector<std::string> const& lines) {
    auto map = read_map(lines);

    while(true) {
        auto&&[next_map, diff] = step(map, &next_step_of);

        if (diff == 0) break;

        map = std::move(next_map);
    }

    return std::count(std::begin(map.cells), std::end(map.cells), Cell::Occupied);
}

long part_2(std::vector<std::string> const& lines) {
    auto map = read_map(lines);

    while(true) {
        auto&&[next_map, diff] = step(map, &next_step_of_variant);

        if (diff == 0) break;

        map = std::move(next_map);
    }

    return std::count(std::begin(map.cells), std::end(map.cells), Cell::Occupied);
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();

    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(lines);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(lines);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(lines));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(lines));
        }
    }
}