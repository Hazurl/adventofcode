#include <iostream>
#include <unordered_set>
#include "../formatted_inputs/day_1"

int main() {
    std::unordered_set<long long> found;
    long long total{};

    bool part_1{false}, part_2{false};

    for(std::size_t i{0};; ++i) {
        if (i >= input.size()) {
            if (! part_1) {
                part_1 = true;
                std::cout << "total: " << total << '\n';
            }

            if (part_2) return 0;

            i = 0;
        }

        if (found.count(total)) {
            part_2 = true;
            std::cout << "got twice " << total << '\n';

            if (part_1) return 0;
        }

        found.insert(total);

        total += input[i];
    }   
}