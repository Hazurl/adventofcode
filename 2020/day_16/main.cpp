#include "../common.hpp"

struct Ticket {
    std::vector<long> values;
};

struct Field {
    std::string name;
    std::vector<std::pair<long, long>> ranges;
};

struct Document {
    std::vector<Field> fields;
    Ticket my_ticket;
    std::vector<Ticket> nearby_tickets;
};

Ticket parse_ticket(std::string const& str) {
    std::stringstream ss(str);
    return { comma_separated<long>(ss) };
}

Field parse_field(std::string const& str) {
    Field f;
    f.name = str.substr(0, str.find(':'));

    std::regex range_regex("(\\d+)-(\\d+)");

    for(auto it = std::sregex_iterator(std::begin(str), std::end(str), range_regex); it != std::sregex_iterator(); ++it) {
        auto match = *it;

        std::pair<long, long> range{ std::stol(match[1].str()), std::stol(match[2].str()) };
        f.ranges.emplace_back(range);
    }

    return f;
}

Document parse_document(std::vector<std::string> const& lines) {
    Document document;

    std::size_t i = 0;
    for(; i < lines.size(); ++i) {
        if (lines[i].empty()) break;

        document.fields.emplace_back(parse_field(lines[i]));
    }

    ++i; // skip empty line
    ++i; // skip "your ticket:"
    document.my_ticket = parse_ticket(lines[i]);
    ++i;

    assert(lines[i].empty());
    ++i; // skip empty line
    ++i; // skip "nearby tickets:"

    for(; i < lines.size(); ++i) {
        document.nearby_tickets.emplace_back(parse_ticket(lines[i]));
    }

    return document;
}

void print_document(Document const& doc) {
    for(auto const& f : doc.fields) {
        std::cout << "Field " << f.name << ": ";
        for(auto[min, max] : f.ranges) {
            std::cout << "[" << min << ", " << max << "] ";
        }
        std::cout << '\n';
    }

    std::cout << '\n';

    auto f = [] (Ticket const& ticket) {
        for(auto value : ticket.values) {
            std::cout << value << ", ";
        }
    };

    std::cout << "My Ticket: ";
    f(doc.my_ticket);
    std::cout << '\n' << '\n';

    std::cout << "Nearby tickets: " << '\n';
    for(auto const& ticket : doc.nearby_tickets) {
        f(ticket);
        std::cout << '\n';
    }
}

struct RangeOptimizer {
    std::vector<std::pair<long, long>> ranges;
    using iterator_t = decltype(std::begin(ranges));

    void add_range(std::pair<long, long> range) {
        if (ranges.empty()) {
            ranges.push_back(range);
            return;
        }

        auto& front = ranges.front();
        if (range.first < front.first) {
            if (range.second + 1 < front.first) {
                ranges.insert(std::begin(ranges), range);
                return;
            } else {
                front.first = std::min(front.first, range.first);
                front.second = std::max(front.second, range.second);

                check_further_overlap(std::begin(ranges));
                return;
            }
        }

        for(std::size_t i = ranges.size(); i > 0; --i) {
            auto it = std::begin(ranges) + (i - 1);
            auto& that = *it; 
            
            if (that.first <= range.first) {
                if(that.second + 1 < range.first) { 
                    it = ranges.insert(it + 1, range);
                    check_further_overlap(it);
                    return;
                } else {
                    that.second = std::max(that.second, range.second);
                    check_further_overlap(it);
                    return;
                }
            }
        }
    }

    void check_further_overlap(iterator_t it) {
        auto first = it;
        auto& combined = *first;
        ++it;

        for(; it != std::end(ranges); ++it) {
            if (it->first <= combined.second) {
                combined.second = std::max(combined.second, it->second);
            } else {
                break;
            }
        }

        ranges.erase(first + 1, it);
    }

    bool includes(long value) const {
        for(auto range : ranges) {
            if (value >= range.first && value <= range.second) {
                return true;
            }
        }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, RangeOptimizer const& that) {
        for(auto range : that.ranges) {
            os << "[" << range.first << ", " << range.second << "] ";
        }
        return os;
    }
};

long part_1(std::vector<std::string> const& lines) {
    auto doc = parse_document(lines);
    RangeOptimizer range_optimizer;

    for(auto const& field : doc.fields) {
        for(auto range : field.ranges) {
            range_optimizer.add_range(range);
        }
    }

    long error = 0;

    for(auto const& ticket : doc.nearby_tickets) {
        for(auto value : ticket.values) {
            if (!range_optimizer.includes((value))) {
                error += value;
            }   
        }
    }

    return error;
}

bool find_fields_order(std::vector<std::vector<std::size_t>> const& fields_by_column, std::vector<bool>& fields_used, std::vector<std::size_t>& order, std::size_t column) {
    if (column >= fields_by_column.size()) {
        return true;
    }

    for(auto field : fields_by_column[column]) {
        if (fields_used[field]) {
            continue;
        }

        fields_used[field] = true;
        if (find_fields_order(fields_by_column, fields_used, order, column + 1)) {
            order[column] = field;
            return true;
        }
        fields_used[field] = false;

    }

    return false;
}

long part_2(std::vector<std::string> const& lines) {
    auto doc = parse_document(lines);
    RangeOptimizer range_optimizer;

    for(auto const& field : doc.fields) {
        for(auto range : field.ranges) {
            range_optimizer.add_range(range);
        }
    }

    std::vector<std::vector<long>> values_by_field(doc.my_ticket.values.size());

    auto add_ticket = [&] (Ticket const& ticket) {
        for(std::size_t i = 0; i < ticket.values.size(); ++i) {
            assert(i < values_by_field.size());
            values_by_field[i].push_back(ticket.values[i]);
        }
    };

    add_ticket(doc.my_ticket);

    for(auto const& ticket : doc.nearby_tickets) {
        bool ticket_valid = true;
        for(auto value : ticket.values) {
            if (!range_optimizer.includes((value))) {
                ticket_valid = false;
                break;
            }   
        }

        if (ticket_valid) {
            add_ticket(ticket);
        }
    }

    std::vector<std::vector<std::size_t>> possible_fields_by_column(values_by_field.size());
    std::vector<RangeOptimizer> field_ranges(doc.fields.size());
    for(std::size_t i = 0; i < field_ranges.size(); ++i) {
        for(auto range : doc.fields[i].ranges) {
            field_ranges[i].add_range(range);
        }
    }

    for(std::size_t i = 0; i < values_by_field.size(); ++i) {
        auto const& values = values_by_field[i];

        for(std::size_t j = 0; j < field_ranges.size(); ++j) {
            auto const& field_range = field_ranges[j];

            bool valid_field = true;
            for(auto value : values) {
                if (!field_range.includes(value)) {
                    valid_field = false;
                    break;
                }
            }

            if (valid_field) {
                possible_fields_by_column[i].push_back(j);
            }
        }
    }

    std::vector<bool> field_used(possible_fields_by_column.size());
    std::vector<std::size_t> order(possible_fields_by_column.size());
    assert(find_fields_order(possible_fields_by_column, field_used, order, 0));

    long departure_product = 1;
    for(std::size_t i = 0; i < order.size(); ++i) {
        auto f = order[i];
        if (doc.fields[f].name.find("departure") != std::string::npos) {
            departure_product *= doc.my_ticket.values[i];
        }
    }

    return departure_product;
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