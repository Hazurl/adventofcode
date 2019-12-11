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

enum class Dir {
    Up, Right, Down, Left
};

Dir rotate_right(Dir d) {
    switch(d) {
        case Dir::Up: return Dir::Right;
        case Dir::Down: return Dir::Left;
        case Dir::Left: return Dir::Up;
        case Dir::Right: return Dir::Down;
    }
    throw std::runtime_error("rotate_right");
}

Dir rotate_left(Dir d) {
    switch(d) {
        case Dir::Down: return Dir::Right;
        case Dir::Up: return Dir::Left;
        case Dir::Right: return Dir::Up;
        case Dir::Left: return Dir::Down;
    }
    throw std::runtime_error("rotate_left!");
}

std::pair<long, long> move_along(std::pair<long, long> const& p, Dir d) {
    switch(d) {
        case Dir::Down: return { p.first, p.second-1 };
        case Dir::Up: return { p.first, p.second+1 };
        case Dir::Right: return { p.first+1, p.second };
        case Dir::Left: return { p.first-1, p.second };
    }
    throw std::runtime_error("move_along!");
}

struct Robot {
    std::pair<long, long> pos = { 0, 0 };
    Dir dir = Dir::Up;
};

}

long part_1(std::vector<long> const& values) {
    std::unordered_map<std::pair<long, long>, bool, decltype([] (auto p) { return size_t(p.first ^ p.second); })> map;

    p1::Robot robot;
    bool waiting_for_paint = true;

    p1::Program program{ values, 
        [&] (unsigned int) { 
            if (map.count(robot.pos) == 0) {
                return map[robot.pos] = false;
            } else {
                return map[robot.pos];
            }
        },
        [&] (unsigned int, long value) { 
            if (waiting_for_paint) {
                map[robot.pos] = value == 0 ? false : true;
            } else { // rotation
                robot.dir = (value == 0 ? p1::rotate_left : p1::rotate_right)(robot.dir);
                robot.pos = p1::move_along(robot.pos, robot.dir);
            }

            waiting_for_paint = !waiting_for_paint;
        } 
    };

    while(!p1::process_once(program));

    return map.size();
}

namespace p2 {

}

long part_2(std::vector<long> const& values) {
    std::unordered_map<std::pair<long, long>, bool, decltype([] (auto p) { return size_t(p.first ^ p.second); })> map;

    p1::Robot robot;
    bool waiting_for_paint = true;

    map[robot.pos] = true;

    p1::Program program{ values, 
        [&] (unsigned int) { 
            if (map.count(robot.pos) == 0) {
                return map[robot.pos] = false;
            } else {
                return map[robot.pos];
            }
        },
        [&] (unsigned int, long value) { 
            if (waiting_for_paint) {
                map[robot.pos] = value == 0 ? false : true;
            } else { // rotation
                robot.dir = (value == 0 ? p1::rotate_left : p1::rotate_right)(robot.dir);
                robot.pos = p1::move_along(robot.pos, robot.dir);
            }

            waiting_for_paint = !waiting_for_paint;
        } 
    };

    while(!p1::process_once(program));

    long min_x = std::numeric_limits<long>::max();
    long min_y = std::numeric_limits<long>::max();
    long max_x = std::numeric_limits<long>::min();
    long max_y = std::numeric_limits<long>::min();

    for(auto const& p : map) {
        min_x = std::min(min_x, p.first.first);
        min_y = std::min(min_y, p.first.second);
        max_x = std::max(max_x, p.first.first);
        max_y = std::max(max_y, p.first.second);
    }

    DBG("X ", min_x, ", ", max_x);
    DBG("Y ", min_y, ", ", max_y);

    for(long y = max_y; y >= min_y; --y) {
        for(long x = min_x; x <= max_x; ++x) {
            std::cout << (map[std::pair{ x, y }] ? "##" : "  ");
        }
        std::cout << '\n';
    }

    return -1;
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