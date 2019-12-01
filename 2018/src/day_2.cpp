#include "../formatted_inputs/day_2.inc"

#include <iostream>

int main() {
    long long global_twice{0};
    long long global_triple{0};

    bool part_2{ false };

    for(std::size_t i{0}; i < input.size(); ++i) {
        if (!part_2) {
            for(std::size_t j{i + 1}; j < input.size(); ++j) {
                long long differ{0};
                std::size_t last{0};
                for(std::size_t k{0}; k < input[i].size(); ++k) {
                    if (input[i][k] != input[j][k]) {
                        last = k;
                        differ++;
                    }
                }

                if (differ == 1) {
                    std::cout << "ID: " << input[i].substr(0, last) << input[i].substr(last + 1) << '\n';
                    part_2 = true;
                    break;
                }
            }
        }

        std::array<int, 26> chars{ 0 };

        long long twice{0};
        long long triple{0};

        for(auto c : input[i]) {
            std::size_t idx = c - 'a';
            chars[idx]++;
            if (chars[idx] == 2) {
                twice++;
            } else if (chars[idx] == 3) {
                twice--;
                triple++;
            } else if (chars[idx] == 4) {
                triple--;
            }
        }

        if (twice) {
            global_twice++;
        }
        if (triple) {
            global_triple++;
        }
    }

    std::cout << "Got " << global_twice << " appearance of letter twice, and " << global_triple << " triple\n";
    std::cout << "=> " << global_twice * global_triple << '\n';
}