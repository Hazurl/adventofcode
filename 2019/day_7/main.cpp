#include "../common.hpp"

namespace p1 {

using stop_t = bool;

struct Program {
    std::vector<long> code;
    std::function<long(unsigned)> inputs;
    unsigned input_count = 0;
    std::size_t pc = 0;
    std::vector<long> outputs = {};


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

        long const& value() const {
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

    //std::cout << '[' << program.pc - 4 << "] ADD " << param << " : " << lhs << " + " << rhs << " -> " << res << '\n';

    res = lhs + rhs; 
}

void mul(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc - 4 << "] MUL " << param << " : " << lhs << " * " << rhs << " -> " << res << '\n';

    res = lhs * rhs; 
}

void input(Program& program, long param) {
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc - 2 << "] INPUT " << param << " : " << res << '\n';

    res = program.input();
}

void output(Program& program, long param) {
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc - 2 << "] OUTPUT " << param << " : " << res << '\n';

    program.output(res);
}

void jmp_true(Program& program, long param) {
    auto cond = get_parameter(next_mode_from(param), program.and_next());
    auto target = get_parameter(next_mode_from(param), program.and_next());
/*
    std::cout << '[' << program.pc - 3 << "] JMP TRUE " << param << " : " << cond << " ? => " << target << '\n';
    std::cout << "[";
    auto tv = target.value();
    for(int i = tv - 3; i <= tv + 3; ++i) {
        std::cout << (tv < program.code.size() && tv >= 0 ? program.code[i] : -1) << ", ";
    }
    std::cout << "]\n";*/

    if (cond != 0) {
        program.pc = target;
    }
}

void jmp_false(Program& program, long param) {
    auto cond = get_parameter(next_mode_from(param), program.and_next());
    auto target = get_parameter(next_mode_from(param), program.and_next());
/*
    std::cout << '[' << program.pc - 3 << "] JMP FALSE " << param << " : not " << cond << " ? => " << target << '\n';
    std::cout << "[";
    auto tv = target.value();
    for(int i = tv - 3; i <= tv + 3; ++i) {
        std::cout << (tv < program.code.size() && tv >= 0 ? program.code[i] : -1) << ", ";
    }
    std::cout << "]\n";*/

    if (cond == 0) {
        program.pc = target;
    }
}

void less_than(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc - 4 << "] LESS " << param << " : " << lhs << " < " << rhs << " -> " << res << '\n';

    res = lhs < rhs ? 1 : 0; 
}

void equals(Program& program, long param) {
    auto lhs = get_parameter(next_mode_from(param), program.and_next());
    auto rhs = get_parameter(next_mode_from(param), program.and_next());
    auto res = get_parameter(next_mode_from(param), program.and_next());

    //std::cout << '[' << program.pc - 4 << "] EQU " << param << " : " << lhs << " == " << rhs << " -> " << res << '\n';

    res = lhs == rhs ? 1 : 0; 
}


stop_t process_once(Program& program) {

    auto instr = program.and_next();
    auto opcode = instr % 100;
    auto param = instr / 100;
    //std::cout << "(execute " << instr.value() << " at " << program.pc - 1 << ")";
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

long execute_amplifier(std::vector<long> const& values, long phase, long signal) {
    p1::Program amp{ values, [&] (unsigned i) {
        return i > 0 ? signal : phase;
    }};

    //DBG(amp.code);

    while(!p1::process_once(amp));

    assert(amp.outputs.size());

    return amp.outputs.empty() ? -1 : amp.outputs.back();
}

}

long part_1(std::vector<long> const& values) {
    std::vector<long> phases = {0, 1, 2, 3, 4};

    long max_e = std::numeric_limits<long>::min();
    std::vector<long> max_phases;

    do {
        long a = p1::execute_amplifier(values, phases[0], 0);
        long b = p1::execute_amplifier(values, phases[1], a);
        long c = p1::execute_amplifier(values, phases[2], b);
        long d = p1::execute_amplifier(values, phases[3], c);
        long e = p1::execute_amplifier(values, phases[4], d);
        if (max_e < e) {
            max_e = e;
            max_phases = phases;
        }

    } while(std::next_permutation(std::begin(phases), std::end(phases)));

    DBG(max_phases);

    return max_e;
}

namespace p2 {

}

long part_2(std::vector<long> const& values) {
    std::vector<long> phases = {5, 6, 7, 8, 9};

    long max_e = std::numeric_limits<long>::min();

    do {
        int index = 0;
        long output = 0;

        std::array amps{
            p1::Program{ values, [&] (unsigned i) { return i > 0 ? output : phases[0]; }},
            p1::Program{ values, [&] (unsigned i) { return i > 0 ? output : phases[1]; }},
            p1::Program{ values, [&] (unsigned i) { return i > 0 ? output : phases[2]; }},
            p1::Program{ values, [&] (unsigned i) { return i > 0 ? output : phases[3]; }},
            p1::Program{ values, [&] (unsigned i) { return i > 0 ? output : phases[4]; }}
        };

        while(!p1::process_once(amps[index])) {
            if (!amps[index].outputs.empty()) {
                std::cout << index << " output " << amps[index].outputs.back() << '\n';
                output = amps[index].outputs.back();
                amps[index].outputs.clear();
                ++index;
                index %= 5;
            }
        }

        max_e = std::max(max_e, output);
    } while(std::next_permutation(std::begin(phases), std::end(phases)));


    return max_e;
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