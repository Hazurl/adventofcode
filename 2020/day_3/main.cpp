#include "../common.hpp"

namespace p1 {
}

long part_1(std::vector<std::string> const& lines) {
    long tree = 0;

    int x = 0;

    for(auto const& row : lines) {
        if (row[x % row.size()] == '#') {
            ++tree;
        }

        x += 3;
    }

    return tree;
}

namespace p2 {
long check_slope(std::vector<std::string> const& lines, int dx, int dy) {
    long tree = 0;

    int x = 0;

    for(int y = 0; y < lines.size(); y += dy) {

        if (lines[y][x % lines[y].size()] == '#') {
            ++tree;
        }

        x += dx;
    }

    return tree;
}
}

long part_2(std::vector<std::string> const& lines) {
    return 
        p2::check_slope(lines, 1, 1) * 
        p2::check_slope(lines, 3, 1) * 
        p2::check_slope(lines, 5, 1) * 
        p2::check_slope(lines, 7, 1) * 
        p2::check_slope(lines, 1, 2)
    ;
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