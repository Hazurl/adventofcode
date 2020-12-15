#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    std::stringstream ss(lines[0]);
    auto values = comma_separated<long>(ss);

    std::unordered_map<long, long> last_occurrence;

    long turn = 1;

    for(auto value : values) {
        last_occurrence[value] = turn;
        ++turn;
    }

    long last = 0;
    
    for(; turn < 2020; ++turn) {
        auto it = last_occurrence.find(last);
        long current = it != std::end(last_occurrence) ? turn - it->second : 0;
        last_occurrence[last] = turn;
        last = current;
    }

    return last;
}

long part_2(std::vector<std::string> const& lines) {
    std::stringstream ss(lines[0]);
    auto values = comma_separated<long>(ss);

    std::unordered_map<long, long> last_occurrence;

    long turn = 1;

    for(auto value : values) {
        last_occurrence[value] = turn;
        ++turn;
    }

    long last = 0;
    
    for(; turn < 30000000; ++turn) {
        auto it = last_occurrence.find(last);
        long current = it != std::end(last_occurrence) ? turn - it->second : 0;
        last_occurrence[last] = turn;
        last = current;
    }

    return last;
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();/*
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
    }*/
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