#include "../common.hpp"

namespace p1 {

using bag_id_t = std::size_t;

}

long part_1(std::vector<std::string> const& lines) {
    std::regex contained_bag_regex("([0-9]+) ([a-zA-Z ]+) bag");
    std::regex container_bag_regex("^([a-zA-Z ]+) bag");

    std::unordered_map<std::string, p1::bag_id_t> name_to_id;
    std::vector<std::vector<p1::bag_id_t>> bags;

    auto get_id_of = [&] (std::string const& name) -> p1::bag_id_t {
        auto it = name_to_id.find(name);
        if (it != std::end(name_to_id)) {
            return it->second;
        } else {
            name_to_id[name] = bags.size();
            bags.push_back({});
            return bags.size() - 1;
        }
    };

    for(auto const& line : lines) {
        std::smatch container_match;
        assert(std::regex_search(line, container_match, container_bag_regex));
        auto bag_id = get_id_of(container_match[1].str());

        for(auto it = std::sregex_iterator(std::begin(line), std::end(line), contained_bag_regex); it != std::sregex_iterator(); ++it) {
            auto match = *it;
            bags[get_id_of(match[2].str())].push_back(bag_id);
        }
    }

    std::unordered_set<p1::bag_id_t> visited;
    std::vector<p1::bag_id_t> to_visit = { get_id_of("shiny gold") };

    int count = 0;

    while(!to_visit.empty()) {
        auto bag_to_visit = to_visit.back();
        to_visit.pop_back();

        for(auto child : bags[bag_to_visit]) {
            if (visited.count(child) <= 0) {
                ++count;
                visited.emplace(child);
                to_visit.push_back(child);
            }
        }
    }

    return count;
}

namespace p2 {

struct Hold {
    int count;
    p1::bag_id_t bag_id;
};

struct Bag {
    std::vector<Hold> hold;
    int total_count = -1;
};

int get_total(p1::bag_id_t id, std::vector<Bag>& bags) {
    auto& bag = bags[id];

    if (bag.total_count != -1) {
        return bag.total_count;
    }

    int total = 0;

    for(auto& hold : bag.hold) {
        total += (get_total(hold.bag_id, bags) + 1) * hold.count;
    }

    bag.total_count = total;
    return total;
}

}

long part_2(std::vector<std::string> const& lines) {
    std::regex contained_bag_regex("([0-9]+) ([a-zA-Z ]+) bag");
    std::regex container_bag_regex("^([a-zA-Z ]+) bag");

    std::unordered_map<std::string, int> name_to_id;
    std::vector<p2::Bag> bags;

    auto get_id_of = [&] (std::string const& name) -> p1::bag_id_t {
        auto it = name_to_id.find(name);
        if (it != std::end(name_to_id)) {
            return it->second;
        } else {
            name_to_id[name] = bags.size();
            bags.push_back({});
            return bags.size() - 1;
        }
    };

    for(auto const& line : lines) {
        std::smatch container_match;
        assert(std::regex_search(line, container_match, container_bag_regex));
        auto bag_id = get_id_of(container_match[1].str());

        for(auto it = std::sregex_iterator(std::begin(line), std::end(line), contained_bag_regex); it != std::sregex_iterator(); ++it) {
            auto match = *it;
            auto hold = p2::Hold{ std::stoi(match[1].str()), get_id_of(match[2].str()) };
            bags[bag_id].hold.push_back(hold);
        }
    }

    return p2::get_total(get_id_of("shiny gold"), bags);
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file, false);
    }();

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
    }

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