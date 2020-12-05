#include "../common.hpp"

namespace p1 {
long parse_id(std::string const& boarding_pass) {
    long value = 0;

    for(int i = 0; i < boarding_pass.size(); ++i) {
        auto c = boarding_pass[i];
        if (c == 'B' || c == 'R') value |= 1 << (boarding_pass.size() - 1 - i);
    }

    return value;
}
}

long part_1(std::vector<std::string> const& lines) {
    long max_id = 0;
    for(auto const& line : lines) {
        auto id = p1::parse_id(line);
        if (id > max_id) {
            max_id = id;
        }
    }

    return max_id;
}

namespace p2 {
}

long part_2(std::vector<std::string> const& lines) {
    std::vector<long> ids(lines.size());

    std::transform(std::begin(lines), std::end(lines), std::begin(ids), p1::parse_id);

    std::sort(std::begin(ids), std::end(ids));

    for(std::size_t i = 0; i < ids.size() - 1; ++i) {
        if (ids[i] + 2 == ids[i + 1]) {
            return ids[i] + 1;
        }
    }

    return -1;
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