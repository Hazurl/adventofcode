#include "../common.hpp"

namespace p1 {

using stop_t = bool;

struct Program {
    std::vector<long> code;
    long cst_input;
    std::size_t pc;
    std::vector<long> outputs;


    struct Cell {
        Program* program;
        std::size_t i;

        Cell operator*() {
            return program->cell_at(operator long&());
        }

        long& operator=(long l) {
            return this->operator long&() = l;
        }

        operator long&() {
            return program->code[i];
        }

        operator long const&() const {
            return program->code[i];
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
        return cst_input;
    }

    void output(long v) {
        outputs.push_back(v);
    }
};

enum class Mode {
    Imm, Pos
};

Mode next_mode_from(long& p) {
    auto m = p % 10;
    p /= 10;
    switch(m) {
        case 0: return Mode::Pos;
        case 1: return Mode::Imm;
    }

    throw std::runtime_error("next_mode_from:: Invalid mode");
}

Program::Cell get_parameter(Mode mode, Program::Cell parameter) {
    switch(mode) {
        case Mode::Imm: return parameter;
        case Mode::Pos: return *parameter;
    }

    throw std::runtime_error("get_parameter:: Invalid mode");
}

void add(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc << "] ADD " << param << " : " << lhs << " + " << rhs << " -> " << res << '\n';

    res = lhs + rhs; 
}

void mul(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc << "] MUL " << param << " : " << lhs << " + " << rhs << " -> " << res << '\n';

    res = lhs * rhs; 
}

void input(Program& program, long param) {
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc << "] INPUT " << param << " : " << res << '\n';

    res = program.input();
}

void output(Program& program, long param) {
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc << "] OUTPUT " << param << " : " << res << '\n';

    program.output(res);
}

void jmp_true(Program& program, long param) {
    auto cond = get_parameter(next_mode_from(param), program.and_next());
    auto target = get_parameter(next_mode_from(param), program.and_next());
    if (cond != 0) {
        program.pc = target;
    }
}

void jmp_false(Program& program, long param) {
    auto cond = get_parameter(next_mode_from(param), program.and_next());
    auto target = get_parameter(next_mode_from(param), program.and_next());
    if (cond == 0) {
        program.pc = target;
    }
}

void less_than(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    res = lhs < rhs ? 1 : 0; 
}

void equals(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    res = lhs == rhs ? 1 : 0; 
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
    }

    return false;
}

}

long part_1(std::vector<long> const& values) {
    p1::Program program{ values, 1, 0 };

    while(!p1::process_once(program));

    return program.outputs.empty() ?-1 : program.outputs.back();
}

namespace p2 {

}

long part_2(std::vector<long> const& values) {
    p1::Program program{ values, 5, 0 };

    while(!p1::process_once(program));

    return program.outputs.empty() ?-1 : program.outputs.back();
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
    }*/

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1] Minimum distance: ", part_1(values));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2] Minimum steps: ", part_2(values));
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