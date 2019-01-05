#include "../formatted_inputs/day_24.inc"
#include <vector>
#include <algorithm>
#include <regex>
#include <cassert>
#include <iostream>

enum class Type {
    Fire, Slash, Bludge, 
    Radiation, Cold
};

struct Group {
    int id;
    bool side;

    int units;

    int init;

    int hp;

    int dmg;
    Type attack_type;

    std::vector<Type> immune_to;
    std::vector<Type> weak_to;
};

Group parse(std::string_view const& s) {
    std::regex rgx("(\\d+) units each with (\\d+) hit points \\(weak to ()\\) with an attack that does 4507 fire damage at initiative 2")
}
(\d+) units each with (\d+) hit points \(immune to ((\w*, )*\w*); weak to ((\w*, )*\w*)\) with an attack that does (\d+) fire damage at initiative (\d+)