#include "../common.hpp"

namespace p1 {

using molecule_t = int;

struct Ingrediant {
    long count;
    molecule_t molecule;
    friend std::ostream& operator<<(std::ostream& os, Ingrediant const& r) {
        return os << r.count << "* #" << r.molecule;
    }
};

struct RecipeResult {
    long count;
    std::vector<Ingrediant> ingrediants;
    friend std::ostream& operator<<(std::ostream& os, RecipeResult const& r) {
        os << r.count << "* #? from ";
        DBG_TO(os, r.ingrediants);
        return os;
    }
};

// molecule * result.count is produced from result.ingrediants
struct Recipe {
    molecule_t molecule;
    RecipeResult result;

    friend std::ostream& operator<<(std::ostream& os, Recipe const& r) {
        os << r.result.count << "* #" << r.molecule << " from ";
        DBG_TO(os, r.result.ingrediants);
        return os;
    }
};


Recipe parce_recipe(std::string str, std::unordered_map<std::string, molecule_t>& molecule_cache) {
    std::regex ing("(\\d+) (\\w+)", std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::vector<Ingrediant> ingrediants;

    std::smatch match;
    while(std::regex_search(str, match, ing)) {
        assert(match.size() == 3);
        Ingrediant i;
        i.count = std::stoi(match[1]);
        i.molecule = molecule_cache.try_emplace(match[2], molecule_cache.size()).first->second;
        ingrediants.push_back(i);
        str = match.suffix();
    }

    auto last = ingrediants.back();
    ingrediants.pop_back();

    Recipe r;
    r.result.ingrediants = std::move(ingrediants);
    r.result.count = last.count;
    r.molecule = last.molecule;

    return r;
}


}

long part_1(std::unordered_map<p1::molecule_t, p1::RecipeResult> const& recipes, p1::molecule_t fuel_id, p1::molecule_t ore_id) {
    std::unordered_map<p1::molecule_t, int> extra;
    std::unordered_map<p1::molecule_t, int> to_consume;

    to_consume.emplace(fuel_id, 1);

    //DBG(recipes);
    //DBG(fuel_id, ", ", ore_id);

    while(true) {
        auto it = std::begin(to_consume);
        while(it->first == ore_id && (++it) != std::end(to_consume));

        if (it == std::end(to_consume)) {
            //std::cout << "END\n";
            break;
        }

        auto[m, c] = *it;
        it = to_consume.erase(it);

        //std::cout << "Consume #" << m << " (" << c << ")\n";

        auto recipe = recipes.at(m);
        if (c < extra[m]) {
            //std::cout << " - Use " << c << " from extra: " << extra[m] << '\n';
            extra[m] -= c;
        } else {
            c -= extra[m];
            auto n = (c + recipe.count - 1) / recipe.count;
            auto cc = (n * recipe.count) - c;

            //std::cout << " - " << extra[m] << " in extra" << '\n';
            //std::cout << " - Can use " << n << " times the recipe: " << c << '\n';
            //std::cout << " - " << cc << " in extra\n";
            extra[m] = cc;

            for(auto const& i : recipe.ingrediants) {
                //std::cout << " - add #" << i.molecule << '*' << i.count * n << '\n';
                to_consume[i.molecule] += i.count * n;
            }

        }
        //std::cin.get();
    }
    return to_consume[ore_id];
}

namespace p2 {

}

long part_2(std::unordered_map<p1::molecule_t, p1::RecipeResult> const& recipes, p1::molecule_t fuel_id, p1::molecule_t ore_id) {
    std::unordered_map<p1::molecule_t, long> extra;
    std::unordered_map<p1::molecule_t, long> to_consume;

    //DBG(recipes);
    //DBG(fuel_id, ", ", ore_id);

    long fuel = 0;
    while(to_consume[ore_id] < 1'000'000'000'000) {
        ++fuel;
        if (fuel % 10000 == 0) std::cout << "FUEL #" << fuel << '\n';
        to_consume.emplace(fuel_id, 1);
        while(true) {
            auto it = std::begin(to_consume);
            while(it->first == ore_id && (++it) != std::end(to_consume));

            if (it == std::end(to_consume)) {
                //std::cout << "END\n";
                break;
            }

            auto[m, c] = *it;
            it = to_consume.erase(it);

            //std::cout << "Consume #" << m << " (" << c << ")\n";

            auto recipe = recipes.at(m);
            if (c < extra[m]) {
                //std::cout << " - Use " << c << " from extra: " << extra[m] << '\n';
                extra[m] -= c;
            } else {
                c -= extra[m];
                auto n = (c + recipe.count - 1) / recipe.count;
                auto cc = (n * recipe.count) - c;

                //std::cout << " - " << extra[m] << " in extra" << '\n';
                //std::cout << " - Can use " << n << " times the recipe: " << c << '\n';
                //std::cout << " - " << cc << " in extra\n";
                extra[m] = cc;

                for(auto const& i : recipe.ingrediants) {
                    //std::cout << " - add #" << i.molecule << '*' << i.count * n << '\n';
                    to_consume[i.molecule] += i.count * n;
                }

            }
            //std::cin.get();
        }
    }
    return fuel - 1;
}

int main() {
    PROFILE_FUNCTION();

    auto&&[values, fuel_id, ore_id] = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        std::unordered_map<std::string, p1::molecule_t> cache;
        std::unordered_map<p1::molecule_t, p1::RecipeResult> recipes;
        for(auto l : lines_of(file)) {
            auto r = p1::parce_recipe(std::move(l), cache);
            recipes.emplace(r.molecule, std::move(r.result));
        }
        return std::tuple{ recipes, cache["FUEL"], cache["ORE"] };
    }();

/*
    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(values, fuel_id, ore_id);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(values, fuel_id, ore_id);
            }
        }
    }*/

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values, fuel_id, ore_id));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values, fuel_id, ore_id));
        }
    }
/*
    {
        PROFILE_PART(1);
        part_1(values, fuel_id, ore_id);
    }

    {
        PROFILE_PART(2);
        part_2(values, fuel_id, ore_id);
    }*/
}