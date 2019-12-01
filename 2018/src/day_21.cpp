#include <iostream>
#include <array>
#include <fstream>
#include <unordered_set>
#include <limits>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <string>

using Registers = std::array<long, 6>;

struct State {
    Registers registers;
    std::size_t ip;
    std::size_t register_bound;
};

struct Args {
    long a;
    long b;
    long c;
};

using op_t = State(*)(State, Args const&);

struct Instruction {
    op_t op;
    Args args;
};

namespace op {

    State addr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " + %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] + input.registers[args.b];
        return input;
    }

    State addi(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " + " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] + args.b;
        return input;
    }




    State mulr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " * %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] * input.registers[args.b];
        return input;
    }

    State muli(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " * " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] * args.b;
        return input;
    }




    State banr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " & %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] & input.registers[args.b];
        return input;
    }

    State bani(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " & " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] & args.b;
        return input;
    }




    State borr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " | %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] | input.registers[args.b];
        return input;
    }

    State bori(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " | " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] | args.b;
        return input;
    }




    State setr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << '\n';
        input.registers[args.c] = input.registers[args.a];
        return input;
    }

    State seti(State input, Args const& args) {
        //std::cout << "%" << args.c << " = " << args.a << '\n';
        input.registers[args.c] = args.a;
        return input;
    }




    State gtir(State input, Args const& args) {
        //std::cout << "%" << args.c << " = " << args.a << " > %" << args.b << '\n';
        input.registers[args.c] = args.a > input.registers[args.b] ? 1 : 0;
        return input;
    }

    State gtri(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " > " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] > args.b ? 1 : 0;
        return input;
    }

    State gtrr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " > %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] > input.registers[args.b] ? 1 : 0;
        return input;
    }




    State eqir(State input, Args const& args) {
        //std::cout << "%" << args.c << " = " << args.a << " == %" << args.b << '\n';
        input.registers[args.c] = args.a == input.registers[args.b] ? 1 : 0;
        return input;
    }

    State eqri(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " == " << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] == args.b ? 1 : 0;
        return input;
    }

    State eqrr(State input, Args const& args) {
        //std::cout << "%" << args.c << " = %" << args.a << " == %" << args.b << '\n';
        input.registers[args.c] = input.registers[args.a] == input.registers[args.b] ? 1 : 0;
        return input;
    }

}

struct Sample {
    Registers before;
    Registers after;
    Instruction instruction;
};

Instruction parse_instruction(std::istream& is, std::unordered_map<std::string, op_t> const& ops) {
    Instruction instruction;
    std::string opname;
    is >> opname;
    instruction.op = ops.at(opname);
    is >> instruction.args.a;
    is >> instruction.args.b;
    is >> instruction.args.c;
    is.get();
    return instruction;
}

std::ostream& operator << (std::ostream& os, State const& r) {
    os << "ip=" << r.ip << " (" << r.register_bound << ") [" << r.registers[0] << ", ";
    os << r.registers[1] << ", " << r.registers[2] << ", " << r.registers[3] << ", ";
    return os << r.registers[4] << ", " << r.registers[5] << ']';
}

std::ostream& operator << (std::ostream& os, Instruction const& i) {
    static std::unordered_map<op_t, std::string_view> ops {
        {op::addr, "addr"},
        {op::addi, "addi"},
        {op::mulr, "mulr"},
        {op::muli, "muli"},
        {op::banr, "banr"},
        {op::bani, "bani"},
        {op::borr, "borr"},
        {op::bori, "bori"},
        {op::setr, "setr"},
        {op::seti, "seti"},
        {op::gtir, "gtir"},
        {op::gtri, "gtri"},
        {op::gtrr, "gtrr"},
        {op::eqir, "eqir"},
        {op::eqri, "eqri"},
        {op::eqrr, "eqrr"},
    };

    return os << ops.at(i.op) << ' ' << i.args.a << ' ' << i.args.b << ' ' << i.args.c;
}

int main() {

    std::ifstream file("formatted_inputs/day_21");
    if (!file) {
        std::cerr << "Oops, can't open file...\n";
        return 1;
    }

    std::unordered_map<std::string, op_t> ops {
        {"addr", op::addr},
        {"addi", op::addi},
        {"mulr", op::mulr},
        {"muli", op::muli},
        {"banr", op::banr},
        {"bani", op::bani},
        {"borr", op::borr},
        {"bori", op::bori},
        {"setr", op::setr},
        {"seti", op::seti},
        {"gtir", op::gtir},
        {"gtri", op::gtri},
        {"gtrr", op::gtrr},
        {"eqir", op::eqir},
        {"eqri", op::eqri},
        {"eqrr", op::eqrr},
    };

    std::vector<Instruction> instructions;

    State state{{0, 0, 0, 0, 0, 0}, 0, 0};

    while(!file.eof()) {
        if (file.peek() == '#') {
            assert(file.get() == '#');
            assert(file.get() == 'i');
            assert(file.get() == 'p');
            assert(file.get() == ' ');
            file >> state.register_bound;
        }
        auto istr = parse_instruction(file, ops);
        instructions.push_back(istr);
        std::cout << instructions.size() -1 << " : " << istr << '\n';
    }

    //bool step = false;

    std::unordered_set<long> values;
    long step{ 0 };

    long min{ std::numeric_limits<long>::max() };
    long max{ 0 };
    long last{ 0 };
    long last2{ 0 };

    while(state.ip < instructions.size()) {
        state.registers[state.register_bound] = state.ip;
        auto const& i = instructions[state.ip];
        state = i.op(state, i.args);

        //std::cout << i << ": " << state << '\n';
        //step = step || state.ip == 28;

        if (state.ip == 28) {
            //std::cout << "> " << state.registers[2] << '\n';
            if (values.count(state.registers[2]) == 0) {
                //std::cout << "B = " << state.registers[2] << " in " << step << " steps\n";
                if (state.registers[2] < min) {
                    //if (max < step) {
                        std::cout << "Minimum: " << state.registers[2] << " in " << step << " steps\n";
                    //}
                    //std::cout << "Minimum: " << state.registers[2] << " in " << step << " steps\n";
                    min = state.registers[2];
                }

                if (max < step) {
                    max = step;
                }
                step = 0;
                values.insert(state.registers[2]);
                last = state.registers[2];
            } else {
                std::cout << "LAST: " << last << '\n';
                std::cout << "LAST2: " << last2 << '\n';
                return 0;
            }

            last2 = state.registers[2];
        }

        state.ip = state.registers[state.register_bound];
        ++state.ip;
        ++step;
    }
}