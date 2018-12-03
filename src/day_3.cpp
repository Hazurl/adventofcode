#include "../formatted_inputs/day_3.inc"

#include <array>
#include <sstream>
#include <cassert>
#include <unordered_set>
#include <iostream>

struct Entry {
    unsigned id;
    unsigned left;
    unsigned top;
    unsigned width;
    unsigned height;
};

Entry parse(std::string_view const& s) {
    std::stringstream is(std::string{s});

    Entry entry;

    assert(is.get() == '#');

    is >> entry.id;

    assert(is.get() == ' ');
    assert(is.get() == '@');
    assert(is.get() == ' ');

    is >> entry.left;
    assert(is.get() == ',');
    is >> entry.top;

    assert(is.get() == ':');
    assert(is.get() == ' ');

    is >> entry.width;
    assert(is.get() == 'x');
    is >> entry.height;

    return entry;
}

int main() {
    std::array<unsigned, 1000*1000> fabric{ 0 };
    std::unordered_set<unsigned> intact;

    unsigned stop{0};

    long long overlaping{0};

    for(auto const& s : input) {
        auto entry = parse(s);
        bool entry_overlaping{ false };


        for(unsigned x{entry.left}; x < entry.left + entry.width; ++x) {
            for(unsigned y{entry.top}; y < entry.top + entry.height; ++y) {
                auto& square = fabric[x * 1000 + y];
                if (square != 0) {
                    entry_overlaping = true;
                }
            }
        }

        for(unsigned x{entry.left}; x < entry.left + entry.width; ++x) {
            for(unsigned y{entry.top}; y < entry.top + entry.height; ++y) {
                auto& square = fabric[x * 1000 + y];
                if (square != 0 && square != ~0u) {
                    ++overlaping;
                    intact.erase(square);
                    square = ~0u;
                } else if (square == 0) {
                    square = entry.id;
                    if (!entry_overlaping) {
                        intact.insert(square);
                    }
                }
            }
        }
    }

    std::cout << "Overlaping: " << overlaping << '\n';
    for(auto id : intact) {
        std::cout << id << " is intact\n";
    }
}