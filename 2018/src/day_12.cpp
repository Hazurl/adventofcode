#include <array>
#include <string_view>
#include <deque>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
/*
static constexpr std::string_view initial_state = "#..#.#..##......###...###";

static constexpr std::array<std::string_view, 14> rules {
    "...## => #",
    "..#.. => #",
    ".#... => #",
    ".#.#. => #",
    ".#.## => #",
    ".##.. => #",
    ".#### => #",
    "#.#.# => #",
    "#.### => #",
    "##.#. => #",
    "##.## => #",
    "###.. => #",
    "###.# => #",
    "####. => #",
};*/

static constexpr std::string_view initial_state = "####..##.##..##..#..###..#....#.######..###########.#...#.##..####.###.#.###.###..#.####..#.#..##..#";

static constexpr std::array<std::string_view, 32> rules {
    ".#.## => .",
    "...## => #",
    "..#.. => .",
    "#.#.. => .",
    "...#. => .",
    ".#... => #",
    "..... => .",
    "#.... => .",
    "#...# => #",
    "###.# => .",
    "..### => #",
    "###.. => .",
    "##.## => .",
    "##.#. => #",
    "..#.# => #",
    ".###. => .",
    ".#.#. => .",
    ".##.. => #",
    ".#### => .",
    "##... => .",
    "##### => .",
    "..##. => .",
    "#.##. => .",
    ".#..# => #",
    "##..# => .",
    "#.#.# => #",
    "#.### => .",
    "....# => .",
    "#..#. => #",
    "#..## => .",
    "####. => #",
    ".##.# => #",
};

struct Rule {
    std::size_t id;
    bool result;
};

Rule parse_rule(std::string_view const& r) {
    Rule rule { 0, false };

    for(int i{ 0 }; i < 5; ++i) {
        rule.id = (rule.id << 1) | (r[i] == '#'); 
    }

    rule.result = r[9] == '#';

    return rule;
}

int main() {
    std::array<bool, 32> parsed_rules { false };
    for(auto const& rule : rules) {
        auto res = parse_rule(rule);
        parsed_rules[res.id] = res.result;
    }
    
    int first_index = 0;
    std::deque<bool> state;
    std::transform(std::begin(initial_state), std::end(initial_state), std::back_inserter(state), [] (auto c) { return c == '#'; });

    long min{ 0 };
    long max{ 0 };

    using key_t = std::deque<bool>;

    auto hasher = [] (std::deque<bool> const& p) {
        std::size_t n = 0;
        for(int i = 0; i < 16 && i < p.size(); ++++i) {
            n ^= p[i] + 0x9e3779b9 + (n << 13) + (n >> 5);
        }

        return n;
    };

    auto equals = [] (key_t const& l, key_t const& r) {
        return l == r;
    };

    struct Pattern {
        std::deque<bool> next_state;
        long from_index;
        long next_index;
    };
    
    std::unordered_map<key_t, Pattern, decltype(hasher), decltype(equals)> patterns(16, hasher, equals);

    //std::unordered_map<key_t, int, decltype(key_hasher), decltype(key_equal)> pattern_gen(16, key_hasher, key_equal);

    for(long gen{ 0 }; gen < 50'000'000'000; ++gen) {
        
        if (first_index < min) min = first_index;
        if (state.size() - 1 > max) max = state.size() - 1;


        if (gen % 1000000 == 0) {
            std::cout << "> " << std::dec << gen << " [" << min << ", " << max << "]\n";

            std::cout << std::setw(16) << std::dec << gen << " [" << std::setw(3) << first_index << "]: ";
            for(auto c : state) {
                std::cout << (c ? '#' : '.');
            }
            std::cout << '\n';
            //std::cin.get();
        }



        /*std::cout << std::setw(16) << std::dec << gen << " [" << std::setw(3) << first_index << "]: ";
        for(auto c : state) {
            std::cout << (c ? '#' : '.');
        }
        std::cout << '\n';*/
/*
        if (patterns.count(state)) {
            //std::cout << "GOTO \n";
            auto& p = patterns[state];
            state = p.next_state;
            first_index += p.next_index - p.from_index;
            continue;
        }*/
        auto last_index = first_index;
        auto last_state = state;


        for(int i{ 2 }; i < last_state.size() - 2; ++i) {
            int id = last_state[i - 2] << 4 | last_state[i - 1] << 3 | last_state[i] << 2 | last_state[i + 1] << 1 | last_state[i + 2];
            assert(id >= 0 && id < parsed_rules.size());
            int res = parsed_rules[id];
            state[i] = res;
        }

        {
            int id{ last_state[0] << 4 | last_state[1] << 3 | last_state[2] << 2 | last_state[3] << 1 | last_state[4] };
            for(int i{ 1 }; i >= -2; --i) {
                id >>= 1;

                assert(id >= 0 && id < parsed_rules.size());
                auto res = parsed_rules[id];

                if (i < 0) {
                    --first_index;
                    state.push_front(res);
                } else {
                    state[i] = res;
                }
            }

            while(!state.empty() && !state[0]) {
                ++first_index;
                state.pop_front();
            }
        }

        {
            auto const size = last_state.size();
            int id{ last_state[size - 5] << 4 | last_state[size - 4] << 3 | last_state[size - 3] << 2 | last_state[size - 2] << 1 | last_state[size - 1] };
            for(int i{ -2 }; i <= 1; ++i) {
                id = (id << 1) & 0b11111;

                assert(id >= 0 && id < parsed_rules.size());
                auto res = parsed_rules[id];

                if (i >= 0) {
                    state.push_back(res);
                } else {
                    state[state.size() + i] = res;
                }
            }

            while(!state.empty() && !state[state.size() - 1]) {
                state.pop_back();
            }
        }

        //patterns[last_state] = Pattern { state, last_index, first_index };
    }

    std::size_t i{ 0 };
    long total{ 0 };
    for(auto c : state) {
        std::cout << (c ? '#' : '.');
        if (c) {
            total += i + first_index;
        }
        ++i;
    }
    std::cout << '\n';

    std::cout << "Part 1: " << std::dec << total << '\n';

}
