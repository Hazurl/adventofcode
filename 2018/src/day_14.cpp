#include <iostream>
#include <vector>
#include <string>

static constexpr int input = 509671;

int main() {
    std::string str = std::to_string(input);
    std::cout << "# " << str << '\n';

    std::size_t e0{ 0 };
    std::size_t e1{ 1 };

    std::vector<int> recipes{ 3, 7 };
    recipes.reserve(input + 10);

    bool part_2{ false };

    while(recipes.size() <= input + 10 || !part_2) {
        auto r0 = recipes[e0];
        auto r1 = recipes[e1];

        auto sum = r0 + r1;

        auto new_r0 = sum / 10;
        auto new_r1 = sum % 10;

        if (new_r0 != 0) {
            recipes.push_back(new_r0);
        }

        recipes.push_back(new_r1);

        e0 = (1 + e0 + r0) % recipes.size();
        e1 = (1 + e1 + r1) % recipes.size();

        if (!part_2 && recipes.size() >= str.size()) {
            part_2 = true;
            for(std::size_t i{ 1 }; i <= str.size(); ++i) {
                if (str.at(str.size() - i) - '0' != recipes.at(recipes.size() - i)) {
                    part_2 = false;
                    break;
                }
            }

            if (part_2) {
                std::cout << "Part2: " << recipes.size() - str.size() << '\n';
            } else if (new_r0 != 0) {
                part_2 = true;
                for(std::size_t i{ 1 }; i <= str.size(); ++i) {
                    if (str.at(str.size() - i) - '0' != recipes.at(recipes.size() - i - 1)) {
                        part_2 = false;
                        break;
                    }
                }

                if (part_2) {
                    std::cout << "Part2: " << recipes.size() - str.size() - 1 << '\n';
                }
            }
        }
    }

    for(std::size_t i{ input }; i < input + 10; ++i) {
        std::cout << recipes[i];
    }
    std::cout << '\n';
}