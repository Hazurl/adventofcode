#include "../common.hpp"

namespace p1 {
}

long part_1(std::vector<std::string> const& lines) {
    long count = 0;

    long count_pairs = 0;
    bool has_cid = false;

    for(auto const& line : lines) {
        if (line.empty()) {
            if (count_pairs - has_cid >= 7) {
                ++count;
            }

            count_pairs = 0;
            has_cid = false;

        } else {
            count_pairs += std::count(std::begin(line), std::end(line), ':');
            has_cid = has_cid || line.find("cid:") != std::string::npos;
        }
    }

    if (count_pairs - has_cid >= 7) {
        ++count;
    }

    return count;
}

namespace p2 {
}

long part_2(std::vector<std::string> const& lines) {
    std::regex pair_regex("([a-z]+):([a-zA-Z0-9#]+)");

    long count = 0;
    bool has_any_invalid_pair = false;
    bool has_cid = false;
    int count_pairs = 0;

    for(auto const& line : lines) {
        if (line.empty()) {
            if (count_pairs - has_cid >= 7 && !has_any_invalid_pair) {
                ++count;
            }

            count_pairs = 0;
            has_cid = false;
            has_any_invalid_pair = false;
            continue;
        }

        for(auto it = std::sregex_iterator(std::begin(line), std::end(line), pair_regex); it != std::sregex_iterator(); ++it) {
            auto match = *it;
            ++count_pairs;

            auto header = match[1].str();
            auto value = match[2].str();

            if(header == "byr") {
                int int_value = std::stoi(value);
                has_any_invalid_pair = has_any_invalid_pair || int_value < 1920 || int_value > 2020 || !std::regex_search(value, std::regex("^[0-9]{4}$"));
            } else if (header == "iyr") {
                int int_value = std::stoi(value);
                has_any_invalid_pair = has_any_invalid_pair || int_value < 2010 || int_value > 2020 || !std::regex_search(value, std::regex("^[0-9]{4}$"));
            } else if (header == "eyr") {
                int int_value = std::stoi(value);
                has_any_invalid_pair = has_any_invalid_pair || int_value < 2020 || int_value > 2030 || !std::regex_search(value, std::regex("^[0-9]{4}$"));
            } else if (header == "hgt") {
                int int_value = std::stoi(value);
                if (value.back() == 'n') {
                    has_any_invalid_pair = has_any_invalid_pair || int_value < 59 || int_value > 76 || !std::regex_search(value, std::regex("^[0-9]+in$"));
                } else {
                    has_any_invalid_pair = has_any_invalid_pair || int_value < 150 || int_value > 193 || !std::regex_search(value, std::regex("^[0-9]+cm$"));
                }
            } else if (header == "hcl") {
                has_any_invalid_pair = has_any_invalid_pair || !std::regex_search(value, std::regex("^#[0-9a-f]{6}$"));
            } else if (header == "ecl") {
                auto values = { "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };
                has_any_invalid_pair = has_any_invalid_pair || std::count(std::begin(values), std::end(values), value) <= 0;
            } else if (header == "pid") {
                has_any_invalid_pair = has_any_invalid_pair || !std::regex_search(value, std::regex("^[0-9]{9}$"));
            } else if (header == "cid") {
                has_cid = true;
            }

        }
    }

    if (count_pairs - has_cid >= 7 && !has_any_invalid_pair) {
        ++count;
    }

    return count;
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