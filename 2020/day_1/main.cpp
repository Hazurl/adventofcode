#include "../common.hpp"

namespace p1 {

}

long part_1(std::vector<std::string> const& lines) {
    std::vector<long> values(lines.size());

    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });
    std::sort(std::begin(values), std::end(values));

    for(auto it = std::begin(values); it != std::end(values); ++it) {
        auto v = *it;
        auto to_find = 2020 - v;

        if (std::binary_search(it + 1, std::end(values), to_find)) {
            return v * to_find;
        }
    }

    throw std::runtime_error("Nothing found");
}

namespace p2 {

}

long part_2(std::vector<std::string> const& lines) {
    std::vector<long> values(lines.size());

    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });
    std::sort(std::begin(values), std::end(values));

    for(auto it = std::begin(values); it != std::end(values); ++it) {
        auto v = *it;
        auto to_find = 2020 - v;

        for(auto it2 = it + 1; it2 != std::end(values); ++it2) {
            auto v2 = *it2;
            auto to_find_final = to_find - v2;

            

            if (std::binary_search(it2 + 1, std::end(values), to_find_final)) {
                return v * v2 * to_find_final;
            }
        }
    }

    throw std::runtime_error("Nothing found");
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