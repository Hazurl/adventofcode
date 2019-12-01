#include "../formatted_inputs/day_5.inc"
//#include <string>
//std::string input = "dabAcCaCBAcCcaDA";

#include <iostream>
#include <array>
#include <cassert>

long prev(std::string const& _input, long i) {
    while(i >= 0 && _input[i] == ' ') --i;
    return i;
}

long next(std::string const& _input, long i) {
    while(i < _input.size() && input[i] == ' ') ++i;
    return i;
}

long react(char to_remove1, char to_remove2) {
    std::string local_input = input;

    long size{ static_cast<long>(local_input.size()) };
    long i{ 0 };


    const char reverse_polarity_diff = 'a' - 'A';

    while(size > 0) {
        long a = prev(local_input, i);
        long b = next(local_input, i + 1);

        while(size > 0) {
            while(b < local_input.size() && (local_input[b] == to_remove1 || local_input[b] == to_remove2)) { 
                --size; 
                local_input[b] = ' ';
                b = next(local_input, b + 1); 
            }

            if (b >= local_input.size()) {
                return size;
            }

            while(a >= 0 && (local_input[a] == to_remove1 || local_input[a] == to_remove2)) { 
                --size; 
                local_input[a] = ' ';
                a = prev(local_input, a - 1);
            }

            if (a < 0) {
                ++i;
                break;
            }

            signed char diff = local_input[a] - local_input[b];
            signed char abs_diff = diff < 0 ? -diff : diff;
            if (abs_diff == reverse_polarity_diff) {
                local_input[a] = ' ';
                local_input[b] = ' ';
                size -= 2;
                a = prev(local_input, a);
                b = next(local_input, b);
            } else {
                ++i;
                break;
            }
        }
    }
    return size;
}

int main() {
    std::cout << "Part 1: " << react(' ', ' ') << '\n';

    long min = input.size();

    for(int i{0}; i < 26; ++i) {
        auto res = react('a' + i, 'A' + i);
        std::cout << "Removing " << (char)('a' + i) << " and " << (char)('A' + i) << ": " << res << '\n';
        if (min > res) min = res;
    }
    std::cout << "Part 2: " << min << '\n';
}