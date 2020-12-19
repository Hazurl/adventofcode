#include "../common.hpp"

struct SubRule {
    std::vector<std::size_t> rules;
};

struct Rule {
    char atom;
    std::vector<SubRule> subrules;
};

struct Document {
    std::vector<Rule> rules;
    std::vector<std::string> messages;
};

std::ostream& operator<<(std::ostream& os, SubRule const& subrule) {
    for(auto id : subrule.rules) {
        os << "#" << id << " ";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, Rule const& rule) {
    if (rule.subrules.empty()) {
        os << "'" << rule.atom << "'";
    } else {
        for(auto const& subrule : rule.subrules) {
            os << subrule << " OR ";
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, Document const& doc) {
    for(std::size_t i = 0; i < doc.rules.size(); ++i) {
        os << "#" << i << " => " << doc.rules[i] << '\n';
    }
    return os;
}

std::pair<std::size_t, Rule> parse_rule(std::string const& line) {
    auto subrules = std::stringstream(line.substr(line.find(':') + 1));
    std::size_t id = std::stol(line);
    Rule rule;

    auto pos_atom = line.find('"');
    if (pos_atom != std::string::npos) {
        rule.atom = line[pos_atom + 1];
        return { id, std::move(rule) };
    }

    for(auto subrule_str : split(subrules, '|')) {
        SubRule subrule;
        std::stringstream ss(subrule_str);

        std::size_t rule_id;
        while(ss >> rule_id) {
            subrule.rules.push_back(rule_id);
        }

        rule.subrules.emplace_back(std::move(subrule));
    }

    return { id, std::move(rule) };
}

Document parse_document(std::vector<std::string> const& lines) {
    Document doc;

    std::size_t i = 0;
    for(; i < lines.size(); ++i) {
        auto const& line = lines[i];
        if(line.empty()) break;

        auto&&[id, rule] = parse_rule(line);
        if (doc.rules.size() <= id) {
            doc.rules.resize(id + 1);
        }
        doc.rules[id] = std::move(rule);
    }

    ++i;

    for(; i < lines.size(); ++i) {
        doc.messages.push_back(lines[i]);
    }

    return doc;
}

std::vector<std::size_t> parse_rule(std::vector<Rule> const& rules, Rule const& rule, std::string const& message, std::size_t begin);

std::vector<std::size_t> parse_subrule(std::vector<Rule> const& rules, SubRule const& subrule, std::string const& message, std::size_t begin) {
    std::vector<std::size_t> positions{ begin };
    for(auto rule : subrule.rules) {
        std::vector<std::size_t> next_positions;
        for(auto pos : positions) {
            auto result_pos = parse_rule(rules, rules[rule], message, pos);
            next_positions.insert(std::end(next_positions), std::begin(result_pos), std::end(result_pos));
        }

        positions = std::move(next_positions);
        if (positions.empty()) return {};
    }

    std::sort(std::begin(positions), std::end(positions));
    positions.erase(std::unique(std::begin(positions), std::end(positions)), std::end(positions));

    return positions;
}

std::vector<std::size_t> parse_rule(std::vector<Rule> const& rules, Rule const& rule, std::string const& message, std::size_t begin) {
    if (rule.subrules.empty()) {
        if (begin < message.size() && message[begin] == rule.atom) {
            return { begin + 1 };
        } else {
            return {};
        }
    } else {
        std::vector<std::size_t> positions;
        for(auto const& subrule : rule.subrules) {
            auto next_positions = parse_subrule(rules, subrule, message, begin);
            positions.insert(std::end(positions), std::begin(next_positions), std::end(next_positions));
        }

        std::sort(std::begin(positions), std::end(positions));
        positions.erase(std::unique(std::begin(positions), std::end(positions)), std::end(positions));

        return positions;
    }
}

bool is_message_valid(std::vector<Rule> const& rules, std::string const& message) {
    auto positions = parse_rule(rules, rules[0], message, 0);
    return std::any_of(std::begin(positions), std::end(positions), [&] (auto p) { return p == message.size(); });
}

long part_1(std::vector<std::string> const& lines) {
    auto doc = parse_document(lines);

    return std::count_if(std::begin(doc.messages), std::end(doc.messages), 
        [&] (auto const& message) { 
            auto is_valid = is_message_valid(doc.rules, message); 
            return is_valid;
        }
    );
}

long part_2(std::vector<std::string> const& lines) {
    auto doc = parse_document(lines);

    doc.rules[8] = Rule{
        {},
        { SubRule{{ 42 }}, SubRule{{ 42, 8 }} },
    };
    doc.rules[11] = Rule{
        {},
        { SubRule{{ 42, 31 }}, SubRule{{ 42, 11, 31 }} },
    };

    return std::count_if(std::begin(doc.messages), std::end(doc.messages), 
        [&] (auto const& message) { 
            auto is_valid = is_message_valid(doc.rules, message); 
            return is_valid;
        }
    );
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file, false);
    }();
    {/*
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
        }*/
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