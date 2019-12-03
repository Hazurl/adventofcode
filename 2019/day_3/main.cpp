#include "../common.hpp"

namespace p1 {

enum class Dir {
    Horizontal, Vertical
};

struct Move {
    Dir dir;
    long dist;

    friend std::istream& operator>>(std::istream& is, Move& m) {
        char c = is.get();
        m.dir = c == 'R' || c == 'L' ? Dir::Horizontal : Dir::Vertical;
        is >> m.dist;
        if (c == 'L' || c == 'U') m.dist = -m.dist;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, Move const& m) {
        return os << (m.dir == Dir::Horizontal ? "Right" : "Down") << "::" << m.dist;
    }

};

struct Vec {
    long x, y;
};

Vec move(Vec const& v, Move const& m) {
    if (m.dir == Dir::Horizontal) {
        return { v.x + m.dist, v.y };
    } else {
        return { v.x, v.y + m.dist };
    }
}

long man_dist(Vec const& lhs, Vec const& rhs) {
    return std::abs(rhs.x - lhs.x) + std::abs(rhs.y - lhs.y);
}

}

long part_1(std::vector<p1::Move> const& wire1, std::vector<p1::Move> const& wire2) {
    p1::Vec pos{0, 0};

    std::unordered_map<long, std::vector<std::pair<long, long>>> horizontal_wires;
    std::unordered_map<long, std::vector<std::pair<long, long>>> vertical_wires;

    for(auto const& m : wire1) {
        auto next_pos = p1::move(pos, m);

        if (m.dir == p1::Dir::Horizontal) {
            auto p = m.dist > 0 ? std::make_pair(pos.x, next_pos.x) : std::make_pair(next_pos.x, pos.x);
            horizontal_wires[pos.y].push_back(p);
        } else {
            auto p = m.dist > 0 ? std::make_pair(pos.y, next_pos.y) : std::make_pair(next_pos.y, pos.y);
            vertical_wires[pos.x].push_back(p);
        }

        pos = next_pos;
    }

    pos = {0, 0};
    long min_d = std::numeric_limits<long>::max();

    for(auto const& m : wire2) {
        auto next_pos = p1::move(pos, m);

        if (m.dir == p1::Dir::Horizontal) {
            auto p = m.dist > 0 ? std::make_pair(pos.x, next_pos.x) : std::make_pair(next_pos.x, pos.x);
            for(long i = p.first + 1; i < p.second - 1; ++i) {
                if(vertical_wires.count(i)) {
                    for(auto const& ws : vertical_wires[i]) {
                        if (ws.first <= pos.y && ws.second >= pos.y) {
                            auto d = p1::man_dist({0, 0}, {i, pos.y});
                            min_d = std::min(min_d, d);
                        }
                    }
                }
            }
        } else {
            auto p = m.dist > 0 ? std::make_pair(pos.y, next_pos.y) : std::make_pair(next_pos.y, pos.y);
            for(long i = p.first + 1; i < p.second - 1; ++i) {
                if(horizontal_wires.count(i)) {
                    for(auto const& ws : horizontal_wires[i]) {
                        if (ws.first <= pos.x && ws.second >= pos.x) {
                            auto d = p1::man_dist({0, 0}, {pos.x, i});
                            min_d = std::min(min_d, d);
                        }
                    }
                }
            }
        }

        pos = next_pos;
    }

    return min_d;
}

namespace p2 {

struct Wire {
    long start, end, start_value;
};

bool intersect_wire(long s, long e, long p) {
    if (s < e) {
        return s <= p && e >= p;
    } else {
        return s >= p && e <= p;
    }
}

}

long part_2(std::vector<p1::Move> const& wire1, std::vector<p1::Move> const& wire2) {
    p1::Vec pos{0, 0};
    long value = 0;

    std::unordered_map<long, std::vector<p2::Wire>> horizontal_wires;
    std::unordered_map<long, std::vector<p2::Wire>> vertical_wires;

    for(auto const& m : wire1) {
        auto next_pos = p1::move(pos, m);

        if (m.dir == p1::Dir::Horizontal) {
            auto p = m.dist > 0 ? p2::Wire{ pos.x, next_pos.x, value } : p2::Wire{ next_pos.x, pos.x, -value };
            horizontal_wires[pos.y].push_back(p);
        } else {
            auto p = m.dist > 0 ? p2::Wire{ pos.y, next_pos.y, value } : p2::Wire{ next_pos.y, pos.y, -value };
            vertical_wires[pos.x].push_back(p);
        }

        pos = next_pos;
        value += std::abs(m.dist);
    }

    pos = {0, 0};
    value = 0;
    long min_value = std::numeric_limits<long>::max();

    for(auto const& m : wire2) {
        auto next_pos = p1::move(pos, m);

        if (m.dir == p1::Dir::Horizontal) {
            auto p = m.dist > 0 ? p2::Wire{ pos.x, next_pos.x, value } : p2::Wire{ next_pos.x, pos.x, -value };
            for(long i = p.start + 1; i < p.end - 1; ++i) {
                if(vertical_wires.count(i)) {
                    for(auto const& ws : vertical_wires[i]) {
                        if (ws.start <= pos.y && ws.end >= pos.y) {
                            long wire1_value = ws.start_value < 0 ? (-ws.start_value + ws.end - pos.y) : (ws.start_value + pos.y - ws.start);
                            long wire2_value = value + std::abs(pos.x - i);
                            min_value = std::min(min_value, wire1_value + wire2_value);
                        }
                    }
                }
            }
        } else {
            auto p = m.dist > 0 ? p2::Wire{ pos.y, next_pos.y, value } : p2::Wire{ next_pos.y, pos.y, -value };
            for(long i = p.start + 1; i < p.end - 1; ++i) {
                if(horizontal_wires.count(i)) {
                    for(auto const& ws : horizontal_wires[i]) {
                        if (ws.start <= pos.x && ws.end >= pos.x) {
                            long wire1_value = ws.start_value < 0 ? (-ws.start_value + ws.end - pos.x) : (ws.start_value + pos.x - ws.start);
                            long wire2_value = value + std::abs(pos.y - i);
                            min_value = std::min(min_value, wire1_value + wire2_value);
                        }
                    }
                }
            }
        }

        pos = next_pos;
        value += std::abs(m.dist);
    }

    return min_value;
}

int main() {
    PROFILE_FUNCTION();

    auto&&[wire1, wire2] = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");

        std::string line1, line2;
        std::getline(file, line1);
        std::getline(file, line2);
        std::stringstream sstream1{ line1 };
        std::stringstream sstream2{ line2 };

        auto wire1 = comma_separated<p1::Move>(sstream1);
        auto wire2 = comma_separated<p1::Move>(sstream2);
        return std::make_pair(std::move(wire1), std::move(wire2));
    }();

    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(wire1, wire2);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(wire1, wire2);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1] Minimum distance: ", part_1(wire1, wire2));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2] Minimum steps: ", part_2(wire1, wire2));
        }
    }

    {
        PROFILE_PART(1);
        part_1(wire1, wire2);
    }

    {
        PROFILE_PART(2);
        part_2(wire1, wire2);
    }
}