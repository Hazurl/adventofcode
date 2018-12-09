#include "../formatted_inputs/day_9.inc"

#include <array>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <list>

struct Config {
    long players;
    long last_marble;
};

Config parse(std::string_view const& s) {
    std::stringstream ss(std::string{s});

    Config config;
    ss >> config.players;
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    ss >> config.last_marble;

    std::cout << "Config: " << config.players << ", " << config.last_marble << '\n';

    return config;
}

long part_1(Config const& c) {
    std::vector<long> scores(c.players, 0);
    std::list<long> marbles = { 0 };
    auto next = ++marbles.begin();
    using iterator_t = decltype(next);

    for(long m{1}; m <= c.last_marble; ++m) {

        if (m % 23 == 0) {
            for(long i = 0; i < 6; ++i) {
                if (next == std::begin(marbles)) {
                    next = std::end(marbles);
                }
                --next;
            }
            auto erased = iterator_t{next};
            for(long i = 0; i < 2; ++i) {
                if (erased == std::begin(marbles)) {
                    erased = std::end(marbles);
                }
                --erased;
            }
            scores[(m - c.players + 1) % c.players] += m + *erased;

            marbles.erase(erased);

        } else {
            for(long i = 0; i < 1; ++i) {
                if (next == std::end(marbles)) {
                    next = std::begin(marbles);
                }
                ++next;
            }
            marbles.insert(next, m);
        }
    } 

    return *std::max_element(std::begin(scores), std::end(scores));
}

int main() {

    auto config = parse(input);
    std::cout << part_1(config) << '\n';
    config.last_marble *= 100;
    std::cout << part_1(config) << '\n';

}