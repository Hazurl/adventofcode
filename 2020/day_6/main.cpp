#include "../common.hpp"

namespace p1 {
}

long part_1(std::vector<std::string> const& lines) {

    std::array<int, 26> count_yes{};
    long sum = 0;

    for(auto const& line : lines) {
        if (line.empty()) {
            sum += std::count_if(std::begin(count_yes), std::end(count_yes), [] (auto v) { return v > 0; });
            count_yes = {};
        } else {
            for(auto c : line) {
                assert(c >= 'a' && c <= 'z');
                ++count_yes[c - 'a'];
            }
        }
    }

    sum += std::count_if(std::begin(count_yes), std::end(count_yes), [] (auto v) { return v > 0; });

    return sum;
}

namespace p2 {
}

long part_2(std::vector<std::string> const& lines) {
    std::array<int, 26> count_yes{};
    long sum = 0;
    long group_size = 0;

    for(auto const& line : lines) {
        if (line.empty()) {
            sum += std::count_if(std::begin(count_yes), std::end(count_yes), [group_size] (auto v) { return v == group_size; });
            count_yes = {};
            group_size = 0;
        } else {
            for(auto c : line) {
                assert(c >= 'a' && c <= 'z');
                ++count_yes[c - 'a'];
            }

            ++group_size;
        }
    }

    sum += std::count_if(std::begin(count_yes), std::end(count_yes), [group_size] (auto v) { return v == group_size; });

    return sum;
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file, false);
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