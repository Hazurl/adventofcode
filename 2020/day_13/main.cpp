#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    long target = std::stol(lines[0]);

    std::stringstream ss(lines[1]);
    auto ids = split(ss, ',');

    auto min_diff = std::numeric_limits<long>::max();
    long best_id = -1;

    for(auto id_str : ids) {
        if (id_str == "x") continue;
        long id = std::stol(id_str);

        auto v = (target + id - 1) / id * id - target;

        if (min_diff > v) {
            best_id = id;
            min_diff = v;
        }
    }

    return best_id * min_diff;
}

long part_2(std::vector<std::string> const& lines) {
    std::stringstream ss(lines[1]);
    auto ids = split(ss, ',');
    std::vector<std::pair<long, long>> schedule;

    for(std::size_t i = 0; i < ids.size(); ++i) {
        auto const& id_str = ids[i];
        if (id_str == "x") continue;

        schedule.push_back({ std::stol(id_str), i });
    }

    long mult = 1;
    long offset = 0;

    auto get_diff = [] (auto id, auto t) {
        return (t + id - 1) / id * id - t;
    };

    for(auto[id, off] : schedule) {
        for(long i = 0; i < mult * id; ++i) {
            auto t = i * mult + offset;
            if (get_diff(id, t) == off % id) {
                mult *= id;
                offset = t % mult;
                break;
            }

        }
    }

    return offset;
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