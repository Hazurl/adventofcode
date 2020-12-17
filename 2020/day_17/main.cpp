#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    std::size_t width = lines[0].size();
    std::size_t height = lines.size();
    std::size_t depth = 1;

    width += 14; // max expansion after 6 cycles
    height += 14;
    depth += 14;

    std::vector<bool> cubes(width * height * depth);
    std::vector<bool> cubes_next_cycle(width * height * depth);

    auto index_of = [&] (std::size_t x, std::size_t y, std::size_t z) {
        return (z * height + y) * width + x;
    };
    auto get = [&] (std::vector<bool>& world, std::size_t x, std::size_t y, std::size_t z) -> decltype(auto) {
        return world[index_of(x, y, z)];
    };

    auto process_cube = [&] (std::size_t x, std::size_t y, std::size_t z) {
        std::size_t count_active_neighbors = 0;

        // x, y and z aren't on the border
        for(std::size_t zc = z - 1; zc <= z + 1; ++zc) {
            for(std::size_t yc = y - 1; yc <= y + 1; ++yc) {
                for(std::size_t xc = x - 1; xc <= x + 1; ++xc) {
                    count_active_neighbors += get(cubes, xc, yc, zc);
                }
            }
        }

        if (get(cubes, x, y, z)) {
            return count_active_neighbors == 3 || count_active_neighbors == 4;
        } else {
            return count_active_neighbors == 3;            
        }
    };

    for(std::size_t z = 0; z < 1; ++z) {
        for(std::size_t y = 0; y < lines.size(); ++y) {
            for(std::size_t x = 0; x < lines[0].size(); ++x) {
                get(cubes, x + 7, y + 7, z + 7) = lines[y][x] == '#';
            }
        }
    }

    for(int cycle = 0; cycle < 6; ++cycle) {
        for(std::size_t z = 1; z < depth - 1; ++z) {
            for(std::size_t y = 1; y < height - 1; ++y) {
                for(std::size_t x = 1; x < width - 1; ++x) {
                    get(cubes_next_cycle, x, y, z) = process_cube(x, y, z);
                }
            }
        }

        std::swap(cubes, cubes_next_cycle);
    }

    return std::count(std::begin(cubes), std::end(cubes), true);
}

long part_2(std::vector<std::string> const& lines) {
    std::size_t width = lines[0].size();
    std::size_t height = lines.size();
    std::size_t depth = 1;
    std::size_t spissitude = 1;

    width += 14; // max expansion after 6 cycles
    height += 14;
    depth += 14;
    spissitude += 14;

    std::vector<bool> cubes(width * height * depth * spissitude);
    std::vector<bool> cubes_next_cycle(width * height * depth * spissitude);

    auto index_of = [&] (std::size_t x, std::size_t y, std::size_t z, std::size_t w) {
        return ((w * depth + z) * height + y) * width + x;
    };
    auto get = [&] (std::vector<bool>& world, std::size_t x, std::size_t y, std::size_t z, std::size_t w) -> decltype(auto) {
        return world[index_of(x, y, z, w)];
    };

    auto process_cube = [&] (std::size_t x, std::size_t y, std::size_t z, std::size_t w) {
        std::size_t count_active_neighbors = 0;

        // x, y and z aren't on the border
        for(std::size_t wc = w - 1; wc <= w + 1; ++wc) {
            for(std::size_t zc = z - 1; zc <= z + 1; ++zc) {
                for(std::size_t yc = y - 1; yc <= y + 1; ++yc) {
                    for(std::size_t xc = x - 1; xc <= x + 1; ++xc) {
                        count_active_neighbors += get(cubes, xc, yc, zc, wc);
                    }
                }
            }
        }

        if (get(cubes, x, y, z, w)) {
            return count_active_neighbors == 3 || count_active_neighbors == 4;
        } else {
            return count_active_neighbors == 3;            
        }
    };

    for(std::size_t w = 0; w < 1; ++w) {
        for(std::size_t z = 0; z < 1; ++z) {
            for(std::size_t y = 0; y < lines.size(); ++y) {
                for(std::size_t x = 0; x < lines[0].size(); ++x) {
                    get(cubes, x + 7, y + 7, z + 7, w + 7) = lines[y][x] == '#';
                }
            }
        }
    }

    for(int cycle = 0; cycle < 6; ++cycle) {
        for(std::size_t w = 1; w < spissitude - 1; ++w) {
            for(std::size_t z = 1; z < depth - 1; ++z) {
                for(std::size_t y = 1; y < height - 1; ++y) {
                    for(std::size_t x = 1; x < width - 1; ++x) {
                        get(cubes_next_cycle, x, y, z, w) = process_cube(x, y, z, w);
                    }
                }
            }
        }

        std::swap(cubes, cubes_next_cycle);
    }

    return std::count(std::begin(cubes), std::end(cubes), true);
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();
    {/*
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
        }*/
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