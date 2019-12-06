#include "../common.hpp"

namespace p1 {

struct Orbit {
    int center;
    int object;
};

Orbit parse_orbit(std::string const& str, std::unordered_map<std::string, int>& cache_id) {
    auto sep_idx = str.find(')');
    assert(sep_idx != std::string::npos);

    auto center = str.substr(0, sep_idx);
    auto object = str.substr(sep_idx + 1);

    auto center_id = cache_id.try_emplace(std::move(center), cache_id.size()).first->second;
    auto object_id = cache_id.try_emplace(std::move(object), cache_id.size()).first->second;

    return {
        center_id, object_id
    };
}

}

long part_1(std::vector<p1::Orbit> const& orbits, int count) {
    // if pred[x] < 0 => x is a root of a group (including itself) of -pred[x] objects
    // if pred[x] >= 0 => x is orbiting around pred[x]
    std::vector<int> pred(count, -1);
    int total = 0;

    for(auto const& o : orbits) {
        // get number of nodes attached to the orbiting object
        int m = -pred[o.object];
        assert(m > 0); // object can only orbit one object
        pred[o.object] = o.center; // set relationship

        // get root
        int* root = &pred[o.center];
        int n = 1;
        // traverse the tree until a root is found
        while(*root >= 0) {
            root = &pred[*root];
            ++n;
        }

        // update number of childs of root
        *root -= m;
        // Each child get n more predecessor
        total += n*m;
    }

    return total;
}

namespace p2 {

}

long part_2(std::vector<p1::Orbit> const& orbits, int count) {
    std::vector<int> pred(count, -1);
    for(auto const& o : orbits) {
        pred[o.object] = o.center;
    }

    std::vector<int> path_to_you;
    int you = 1;
    while(pred[you] >= 0) {
        you = pred[you];
        path_to_you.push_back(you);
    }

    std::vector<int> path_to_santa;
    int santa = 2;
    while(pred[santa] >= 0) {
        santa = pred[santa];
        path_to_santa.push_back(santa);
    }

    std::size_t i = 0;
    for(; i < std::min(path_to_you.size(), path_to_santa.size()) && path_to_you[path_to_you.size() - i - 1] == path_to_santa[path_to_santa.size() - i - 1]; ++i);

    return path_to_you.size() - i + path_to_santa.size() - i;
}

int main() {
    PROFILE_FUNCTION();

    auto&&[values, count] = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        auto lines = lines_of(file);
        std::vector<p1::Orbit> orbits;
        orbits.reserve(lines.size());
        std::unordered_map<std::string, int> ids;
        ids.emplace("COM", 0);
        ids.emplace("YOU", 1);
        ids.emplace("SAN", 2);
        std::transform(std::begin(lines), std::end(lines), std::back_inserter(orbits), [&] (auto const& l) { return p1::parse_orbit(l, ids); });
        return std::pair{ orbits, ids.size() };
    }();


    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(values, count);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(values, count);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values, count));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values, count));
        }
    }

    {
        PROFILE_PART(1);
        part_1(values, count);
    }

    {
        PROFILE_PART(2);
        part_2(values, count);
    }
}