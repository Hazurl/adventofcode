#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    std::vector<long> values(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });

    for(std::size_t i = 25; i < lines.size(); ++i) {
        bool found_pair = false;

        for(std::size_t j = i - 25; j < i - 1; ++j) {
            for(std::size_t k = j + 1; k < i; ++k) {
                if (values[j] + values[k] == values[i]) {
                    found_pair = true;
                    break;
                }
            }

            if (found_pair) break;
        }

        if (!found_pair) {
            return values[i];
        }
    }

    return -1;
}

std::optional<long> solve_at(std::vector<long> const& values, std::size_t idx, long sum) {
    long min = std::numeric_limits<long>::max();
    long max = std::numeric_limits<long>::min();

    long count = 0;

    for(; idx < values.size(); ++idx) {
        ++count;
        sum -= values[idx];

        min = std::min(min, values[idx]);
        max = std::max(max, values[idx]);

        if (sum == 0 && count >= 2) {
            return min + max;
        }

        if (sum < 0) {
            return std::nullopt;
        }
    }

    return std::nullopt;
}

long part_2(std::vector<std::string> const& lines) {
    constexpr long anomaly = 90433990;

    std::vector<long> values(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });

    for(std::size_t i = 0; i < values.size(); ++i) {
        auto res = solve_at(values, i, anomaly);
        if (res) {
            return *res;
        }
    }

    return -1;
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