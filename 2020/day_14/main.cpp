#include "../common.hpp"

using u36 = unsigned long;
using mask_t = std::pair<u36, u36>;

constexpr u36 u36_mask = 0xF'FF'FF'FF'FF;

mask_t get_mask(std::string const& str) {
    u36 mask_and = ~0;
    u36 mask_or = 0;

    auto start = str.find('=') + 2;
    auto end = start + 36;
    assert(end <= str.size());

    for(auto pos = start; pos < end; ++pos) {
        auto c = str[pos];
        u36 bit = 1ul << (35 - (pos - start));
        if (c == '0') mask_and &= ~bit;
        if (c == '1') mask_or |= bit;
    }

    return { mask_and & u36_mask, mask_or & u36_mask };
}

u36 apply_mask(mask_t mask, u36 value) {
    return (value & mask.first) | mask.second;
}

struct Instruction {
    u36 address;
    u36 value_to_write;
};

Instruction parse_instruction(std::string const& str) {
    auto const pos_addr = str.find('[') + 1;
    auto const pos_value = str.find('=') + 2;

    auto const c_str = str.c_str();
    auto const addr = std::stol(c_str + pos_addr) & u36_mask;
    auto const value = std::stol(c_str + pos_value) & u36_mask;

    return { addr, value };
}

long part_1(std::vector<std::string> const& lines) {
    mask_t mask{ ~0, 0 };

    std::unordered_map<u36, u36> memory;

    for(auto const& line : lines) {
        if (line[1] == 'a') {
            mask = get_mask(line);
        } else {
            auto instruction = parse_instruction(line);
            auto masked = apply_mask(mask, instruction.value_to_write);
            memory[instruction.address] = masked;
        }
    }

    return std::accumulate(std::begin(memory), std::end(memory), 0ul, [] (auto acc, auto const& m) { return acc + m.second; });
}

using floating_mask_t = std::pair<u36, u36>;

mask_t get_floating_mask(std::string const& str) {
    u36 mask_or = 0;
    u36 mask_floating = 0;

    auto start = str.find('=') + 2;
    auto end = start + 36;
    assert(end <= str.size());

    for(auto pos = start; pos < end; ++pos) {
        auto c = str[pos];
        u36 bit = 1ul << (35 - (pos - start));
        if (c == 'X') mask_floating |= bit;
        if (c == '1') mask_or |= bit;
    }

    return { mask_or & u36_mask, mask_floating & u36_mask };
}

template<typename F>
void enumerate_values_helper(u36 floating_mask, u36 index, u36 initial, F visitor) {
    if (index >= 36) {
        visitor(initial);
        return;
    }

    if (floating_mask & (1ul << index)) {
        enumerate_values_helper(floating_mask, index + 1, initial & ~(1ul << index), visitor);
        enumerate_values_helper(floating_mask, index + 1, initial | (1ul << index), visitor);
    } else {
        enumerate_values_helper(floating_mask, index + 1, initial, visitor);
    }
}

template<typename F>
void enumerate_values(floating_mask_t mask, u36 initial, F visitor) {
    initial |= mask.first;
    enumerate_values_helper(mask.second, 0, initial, visitor);
}


long part_2(std::vector<std::string> const& lines) {
    floating_mask_t mask{ 0, 0 };

    std::unordered_map<u36, u36> memory;

    for(auto const& line : lines) {
        if (line[1] == 'a') {
            mask = get_floating_mask(line);
        } else {
            auto instruction = parse_instruction(line);
            enumerate_values(mask, instruction.address, [&] (auto masked) {
                memory[masked] = instruction.value_to_write;
            });
        }
    }

    return std::accumulate(std::begin(memory), std::end(memory), 0ul, [] (auto acc, auto const& m) { return acc + m.second; });
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