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

enum class Block {
    Empty, Wall, Block, Paddle, Ball
};

}

long part_1(std::vector<long> const& values) {
    long x, y;
    long count = 0;
    p1::Program program{ values, 
        [] (unsigned int) -> long { 
            throw std::runtime_error("Should not call 'input'");
        },
        [&] (unsigned int i, long v) { 
            switch(i % 3) {
                case 0: x = v; break;
                case 1: y = v; break;
                case 2: {
                    std::cout << x << ", " << y << ": " << v << '\n';
                    if (static_cast<p1::Block>(v) == p1::Block::Block) {
                        ++count;
                    }
                }
            }
        } 
    };

    while(!p1::process_once(program));

    return count;
}

namespace p2 {

}

long part_2(std::vector<long> const& values) {
    long x, y;
    long score;
    long bx, by;
    long px, py;

    p1::Program program{ values, 
        [&] (unsigned int) { 
            return px < bx ? 1 : px > bx ? -1 : 0;
        },
        [&] (unsigned int i, long v) { 
            switch(i % 3) {
                case 0: x = v; break;
                case 1: y = v; break;
                case 2: {
                    if (x == -1 && y == 0) score = v;
                    else if (static_cast<p1::Block>(v) == p1::Block::Ball) {
                        bx = x;
                        by = y;
                    }
                    else if (static_cast<p1::Block>(v) == p1::Block::Paddle) {
                        px = x;
                        py = y;
                    }
                }
            }
        } 
    };

    program.code[0] = 2;

    while(!p1::process_once(program));

    return score;
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