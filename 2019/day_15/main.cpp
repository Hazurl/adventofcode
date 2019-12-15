#include "../common.hpp"

namespace p1 {

using stop_t = bool;

enum class Mode {
    Imm, Pos, Rel
};

Mode next_mode_from(long& p) {
    auto m = p % 10;
    p /= 10;
    switch(m) {
        case 0: return Mode::Pos;
        case 1: return Mode::Imm;
        case 2: return Mode::Rel;
    }

    throw std::runtime_error("next_mode_from:: Invalid mode");
}

struct Program {
    std::vector<long> code;
    std::function<long(unsigned)> inputs;
    std::function<void(unsigned, long)> outputs;
    unsigned input_count = 0;
    unsigned output_count = 0;
    std::size_t pc = 0;
    long relative_base = 0;

    bool debug_enable = false;

    struct Cell {
        Program* program;
        std::size_t i;

        Cell operator*() {
            return program->cell_at(operator long&());
        }

        Cell operator[](long rel) {
            if (i >= program->code.size()) {
                program->code.resize(i + 1, 0);
            }
            return program->cell_at(program->code.at(i) + rel);
        }

        long& operator=(long l) {
            return value() = l;
        }

        operator long&() {
            if (i >= program->code.size()) {
                program->code.resize(i + 1, 0);
            }
            return program->code.at(i);
        }

        operator long() const {
            if (i >= program->code.size()) {
                return 0;
            }
            return program->code.at(i);
        }

        long value() const {
            return *this;
        }

        long& value() {
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, Cell const& cell) {
            return os << '#' << cell.i << "(" << static_cast<long>(cell) << ')';
        }
    };

    Cell cell_at(std::size_t i) {
        return {
            this, i
        };
    }

    Cell operator*() {
        return cell_at(pc);
    }

    Cell and_next() {
        auto c = **this;
        ++pc;
        return c; 
    }

    long input() {
        return inputs(input_count++);
    }

    void output(long v) {
        outputs(output_count++, v);
    }

    void debug(const char* instr, unsigned n) {
        if (debug_enable) {
            auto old_pc = pc;
            auto modes = cell_at(pc - 1) / 100;
            std::cout << pc - 1 << ": " << instr << " (" << modes << ") ";
            while(n--) {
                auto m = next_mode_from(modes);
                auto p = and_next();
                switch(m) {
                    case Mode::Imm: std::cout << "#" << p.value(); break;
                    case Mode::Pos: std::cout << "[" << p.value() << "](#" << (*p).value() << ")"; break;
                    case Mode::Rel: std::cout << "[" << p.value() << "+" << relative_base << "](#" << p[relative_base].value() << ")"; break;
                }
                if (n > 0) {
                    std::cout << ", ";
                }
            }
            pc = old_pc;      
            std::cout << '\n';
        }
    }
};

Program::Cell get_parameter(Mode mode, Program::Cell parameter, long relative_base) {
    switch(mode) {
        case Mode::Imm: return parameter;
        case Mode::Pos: return *parameter;
        case Mode::Rel: return parameter[relative_base]; 
    }

    throw std::runtime_error("get_parameter:: Invalid mode");
}

void add(Program& program, long param) {
    program.debug("add", 3);

    auto lhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto rhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    res = lhs + rhs; 
}

void mul(Program& program, long param) {
    program.debug("mul", 3);

    auto lhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto rhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    res = lhs * rhs; 
}

void input(Program& program, long param) {
    program.debug("inp", 1);

    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    res = program.input();
}

void output(Program& program, long param) {
    program.debug("out", 1);

    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    program.output(res);
}

void jmp_true(Program& program, long param) {
    program.debug("jmt", 2);

    auto cond = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto target = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    if (cond != 0) {
        program.pc = target;
    }
}

void jmp_false(Program& program, long param) {
    program.debug("jmf", 2);

    auto cond = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto target = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    if (cond == 0) {
        program.pc = target;
    }
}

void less_than(Program& program, long param) {
    program.debug("lst", 3);

    auto lhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto rhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    res = lhs < rhs ? 1 : 0; 
}

void equals(Program& program, long param) {
    program.debug("equ", 3);

    auto lhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto rhs = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);
    auto res = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    res = lhs == rhs ? 1 : 0; 
}

void add_to_relative_base(Program& program, long param) {
    program.debug("rel", 1);

    auto val = get_parameter(next_mode_from(param), program.and_next(), program.relative_base);

    program.relative_base += val;
}


stop_t process_once(Program& program) {

    auto instr = program.and_next();
    auto opcode = instr % 100;
    auto param = instr / 100;

    switch(opcode) {
        case 99: return true;
        default: return false;

        case 1: add(program, param); break;
        case 2: mul(program, param); break;
        case 3: input(program, param); break;
        case 4: output(program, param); break;
        case 5: jmp_true(program, param); break;
        case 6: jmp_false(program, param); break;
        case 7: less_than(program, param); break;
        case 8: equals(program, param); break;
        case 9: add_to_relative_base(program, param); break;
    }

    return false;
}

struct Hasher {
    std::size_t operator()(std::pair<long, long> const& p) const {
        return p.first ^ p.second;
    }
};

int dist(std::pair<long, long> const& from, std::pair<long, long> const& to) {
    return std::abs(from.first - to.first) + std::abs(from.second - to.second);
}

std::pair<long, long> move(std::pair<long, long> p, int dir) {
    auto last = p;
    switch(dir) {
        case 1: p.second++; break;
        case 2: p.second--; break;
        case 3: p.first++; break;
        case 4: p.first--; break;
    }
    DBG("MOVE ", last, ", ", dir, " => ", p);
    return p;
}

int inverse_direction(int dir) {
    switch(dir) {
        case 1: return 2;
        case 2: return 1;
        case 3: return 4;
        case 4: return 3;
    }
    std::cerr << "inverse_direction! " << dir << '\n';
    throw std::runtime_error("inverse_direction!");
}

int dir_from_cells(std::pair<long, long> const& from, std::pair<long, long> const& to) {
    if (from.first < to.first) {
        return 3;
    }

    if (from.first > to.first) {
        return 4;
    }

    if (from.second > to.second) {
        return 2;
    }

    return 1;
}

std::vector<int> pathfinding(std::unordered_map<std::pair<long, long>, bool, p1::Hasher> const& walls, std::pair<long, long> const& from, std::pair<long, long> const& to) {
    std::unordered_set<std::pair<long, long>, Hasher> closed = {};
    std::unordered_map<std::pair<long, long>, std::pair<int, int>, Hasher> open = {};

    std::unordered_map<std::pair<long, long>, int, Hasher> parent = {};

    open.emplace(from, std::pair{ dist(from, to), 0 });

    std::vector<int> result;

    while (!open.empty()) {
        auto pair = std::min_element(open.begin(), open.end(), [] (auto const& lhs, auto const& rhs) -> bool  {
            return lhs.second.first + lhs.second.second < rhs.second.first + rhs.second.second;
        });

        std::pair<long, long> cur = pair->first;
        auto [heursitic, cost] = pair->second;
        open.erase(pair);

        DBG("  Current: ", cur);

        closed.emplace(cur);

        if (cur == to) {
            auto current = to;

            DBG("  FINISHED");

            do {
                int dir = parent.at(current);
                result.push_back(inverse_direction(dir));
                DBG(current, " <- ", move(current, dir), " with ", dir);
                current = move(current, dir);
            } while (current != from);

            return result;
        }

        auto neighbours = {
            std::pair{ cur.first+1, cur.second },
            std::pair{ cur.first-1, cur.second },
            std::pair{ cur.first, cur.second+1 },
            std::pair{ cur.first, cur.second-1 }
        };

        for (auto const& neighbour : neighbours) {
            if (neighbour != to && (walls.count(neighbour) == 0 || walls.at(neighbour))) {
                continue;
            }

            if (closed.find(neighbour) != closed.end()) {
                continue;
            }

            DBG("   - Checking ", neighbour);

            auto it_open = open.find(neighbour);
            int new_cost = cost + dist(cur, neighbour);

            if (it_open == open.end() || new_cost < it_open->second.second) {
                std::pair new_weight{ dist(neighbour, to), new_cost };
                DBG(" - - Weight: ", new_weight);

                if (it_open != open.end()) {
                    open.erase(it_open);
                }

                open.emplace(neighbour, new_weight);

                DBG("   - - Parent of ", neighbour, " is ", cur, " in dir ", dir_from_cells(neighbour, cur));
                parent[neighbour] = dir_from_cells(neighbour, cur);
            }
        }
    }

    return {};
}

}

long part_1(std::vector<long> const& values) {
    std::pair<long, long> pos{0, 0};
    std::unordered_map<std::pair<long, long>, bool, p1::Hasher> walls;
    std::vector<std::pair<long, long>> to_visit;
    std::pair<long, long> visiting;
    std::vector<int> path;
    int moving;

    to_visit.push_back({1, 0});
    to_visit.push_back({0, -1});
    to_visit.push_back({-1, 0});
    visiting = {0, 1};

    path.push_back(1);
    walls[pos] = false;

    std::optional<int> result;
    std::optional<std::pair<long, long>> goal;

    p1::Program program{ values, 
        [&] (unsigned int) -> long { 
            if (path.empty()) {
                assert(!to_visit.empty());
                visiting = to_visit.back();
                to_visit.pop_back();

                path = p1::pathfinding(walls, pos, visiting);
                DBG("FROM ", pos, " TO ", visiting, "; PATH: ", path);
            }

            assert(!path.empty());
            int m = path.back();
            path.pop_back();
            moving = m;
            DBG("SEND ", m);
            return m;
        },
        [&] (unsigned int, long v) { 
            switch(v) {
                case 0: {
                    walls[visiting] = true;
                    DBG(visiting, " is a wall");
                    assert(path.empty());
                    break;
                }
                default: {
                    pos = p1::move(pos, moving);
                    if (pos == visiting) {
                        DBG(visiting, " is discovered");
                        walls[visiting] = false;

                        if (walls.count(p1::move(visiting, 1)) == 0) to_visit.push_back(p1::move(visiting, 1));
                        if (walls.count(p1::move(visiting, 2)) == 0) to_visit.push_back(p1::move(visiting, 2));
                        if (walls.count(p1::move(visiting, 3)) == 0) to_visit.push_back(p1::move(visiting, 3));
                        if (walls.count(p1::move(visiting, 4)) == 0) to_visit.push_back(p1::move(visiting, 4));

                        assert(path.empty());
                    }
                    break;
                }
            }
            if (v == 2) {
                DBG("FOUND IT AT ", pos);
                goal = pos;
                result = p1::pathfinding(walls, {0, 0}, pos).size();
            }
        } 
    };

    while(!p1::process_once(program) && !to_visit.empty());

    {
        int mix = std::numeric_limits<int>::max();
        int miy = std::numeric_limits<int>::max();
        int max = std::numeric_limits<int>::min();
        int may = std::numeric_limits<int>::min();
        for(auto[pos, _] : walls) {
            if (pos.first < mix) {
                mix = pos.first;
            } else if (pos.first > max) {
                max = pos.first;
            }
            if (pos.second < miy) {
                miy = pos.second;
            } else if (pos.second > may) {
                may = pos.second;
            }
        }

        for(int y = miy; y < may; ++y) {
            for(int x = mix; x < max; ++x) {
                if (x == 0 && y == 0) {
                    std::cout << "@@";
                } else if (x == goal->first && y == goal->second) {
                    std::cout << "$$";
                } else if (walls.count({x, y}) == 0) {
                    std::cout << "  ";
                } else if (walls[{x, y}]) {
                    std::cout << "##";
                } else {
                    std::cout << "..";
                }
            }
            std::cout << '\n';
        }
    }
    return *result;
}

namespace p2 {

int find_answer(std::unordered_map<std::pair<long, long>, bool, p1::Hasher> const& walls, std::pair<long, long> const& goal) {
    std::unordered_map<std::pair<long, long>, int, p1::Hasher> with_oxygen;
    with_oxygen.emplace(goal, 0);

    std::vector<std::pair<long, long>> to_visit = { goal };
    std::vector<std::pair<long, long>> to_visit_next;

    int count = 0;

    while(!to_visit.empty()) {
        for(auto cur : to_visit) {
            auto neighbours = {
                std::pair{ cur.first+1, cur.second },
                std::pair{ cur.first-1, cur.second },
                std::pair{ cur.first, cur.second+1 },
                std::pair{ cur.first, cur.second-1 }
            };

            for (auto const& neighbour : neighbours) {
                if (walls.at(neighbour)) {
                    continue;
                }

                if (with_oxygen.count(neighbour) == 0) {
                    to_visit_next.push_back(neighbour);
                    with_oxygen[neighbour] = with_oxygen[cur] + 1;
                    count = std::max(count, with_oxygen[neighbour]);
                }
            }
        }

        std::swap(to_visit, to_visit_next);
        to_visit_next.clear();
    }
        
    return count;
}

}

long part_2(std::vector<long> const& values) {
    std::pair<long, long> pos{0, 0};
    std::unordered_map<std::pair<long, long>, bool, p1::Hasher> walls;
    std::vector<std::pair<long, long>> to_visit;
    std::pair<long, long> visiting;
    std::vector<int> path;
    int moving;

    to_visit.push_back({1, 0});
    to_visit.push_back({0, -1});
    to_visit.push_back({-1, 0});
    visiting = {0, 1};

    path.push_back(1);
    walls[pos] = false;

    std::pair<long, long> goal;

    p1::Program program{ values, 
        [&] (unsigned int) -> long { 
            if (path.empty()) {
                assert(!to_visit.empty());
                visiting = to_visit.back();
                to_visit.pop_back();

                path = p1::pathfinding(walls, pos, visiting);
                DBG("FROM ", pos, " TO ", visiting, "; PATH: ", path);
            }

            assert(!path.empty());
            int m = path.back();
            path.pop_back();
            moving = m;
            DBG("SEND ", m);
            return m;
        },
        [&] (unsigned int, long v) { 
            switch(v) {
                case 0: {
                    walls[visiting] = true;
                    DBG(visiting, " is a wall");
                    assert(path.empty());
                    break;
                }
                default: {
                    pos = p1::move(pos, moving);
                    if (pos == visiting) {
                        DBG(visiting, " is discovered");
                        walls[visiting] = false;

                        if (walls.count(p1::move(visiting, 1)) == 0) to_visit.push_back(p1::move(visiting, 1));
                        if (walls.count(p1::move(visiting, 2)) == 0) to_visit.push_back(p1::move(visiting, 2));
                        if (walls.count(p1::move(visiting, 3)) == 0) to_visit.push_back(p1::move(visiting, 3));
                        if (walls.count(p1::move(visiting, 4)) == 0) to_visit.push_back(p1::move(visiting, 4));

                        assert(path.empty());
                    }
                    break;
                }
            }
            if (v == 2) {
                DBG("FOUND IT AT ", pos);
                goal = pos;
            }
        } 
    };

    while(!p1::process_once(program) && !to_visit.empty());

    return p2::find_answer(walls, goal);
}

int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return comma_separated<long>(file);
    }();

/*
    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(values);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(values);
            }
        }
    }
*/
    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values));
        }
    }
/*
    {
        PROFILE_PART(1);
        part_1(values);
    }

    {
        PROFILE_PART(2);
        part_2(values);
    }*/
}