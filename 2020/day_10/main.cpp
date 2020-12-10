#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    std::vector<long> values(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });

    std::sort(std::begin(values), std::end(values));

    std::array<long, 4> diff_count{ 0, 0, 0, 1 };

    assert(values.front() >= 1 && values.front() <= 3);
    ++diff_count[values.front()];

    for(std::size_t i = 1; i < values.size(); ++i) {
        auto that = values[i];
        auto other = values[i - 1];
        auto diff = that - other;
        assert(diff >= 1 && diff <= 3);
        ++diff_count[diff];
    }

    return diff_count[1] * diff_count[3];
}

long part_2(std::vector<std::string> const& lines) {
    std::vector<long> values(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(values), [] (auto const& str) { return std::stol(str); });

    std::sort(std::begin(values), std::end(values));

    std::vector<long> arrangements(values.size());
    arrangements.back() = 1;

    for(std::size_t i = values.size(); i > 0; --i) {
        auto idx = i - 1;
        auto value = values[idx];

        auto& arrangement = arrangements[idx];
        for(std::size_t j = i; j < i + 3 && j < values.size(); ++j) {
            if (values[j] - value > 3) break;

            arrangement += arrangements[j];
        }
    }

    std::size_t total = 0;
    for(std::size_t i = 0; i < 3; ++i) {
        if (values[i] > 3) break;
        total += arrangements[i];
    }

    return total;
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