#include <iostream>
#include <array>
#include <fstream>
#include <unordered_set>
#include <limits>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <string>

using Registers = std::array<int, 6>;

struct State {
    Registers registers;
    std::size_t ip;
    std::size_t register_bound;
};

struct Args {
    int a;
    int b;
    int c;
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

int part_1() {
    int a{ 0 }, b{ 0 }, c{ 0 }, d{ 0 }, e{ 0 };

    e += 2;
    e *= e;
    e *= 19;
    e *= 11;
    b += 3;
    b *= 22;
    b += 12;
    e += b;

    c = 1;
    do {
        d = 1;
        do {
            b = c * d;
            if (b == e) {
                a += c;
            }
            d += 1;

        } while(d <= e);

        c += 1;
    } while(c <= e);

    return a;
}

long part_2() {
    long a{ 1 }, b{ 0 }, c{ 0 }, d{ 0 }, e{ 0 };

    e += 2;
    e *= e;
    e *= 19;
    e *= 11;
    b += 3;
    b *= 22;
    b += 12;
    e += b;

    b = 27;
    b *= 28;
    b += 29;
    b *= 30;
    b *= 14;
    b *= 32;
    e += b;
    a = 0;

    std::cout << "E= " << e << '\n';

    int p = 0;

    c = 1;
    do {
        d = 1;
        do {
            b = c * d;
            if (b == e) {
                std::cout << "> " << c << " * " << d << '\n';
                a += c;
            }
            d += 1;

        } while(d <= e);

        c += 1;
        if (c * 10000 / e > p) {
            p = c * 10000 / e;
            std::cout << p / 100. << "%\n";
        }
    } while(c <= e);

    return a;
}

int main() {

    std::ifstream file("formatted_inputs/day_19");
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

    State state{{0, 0, 0, 0}, 0, 0};

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

    std::cout << "My part 1: " << part_1() << '\n';
    std::cout << "My part 2: " << part_2() << '\n';
    std::cin.get();

    while(state.ip < instructions.size()) {
        state.registers[state.register_bound] = state.ip;
        auto const& i = instructions[state.ip];
        state = i.op(state, i.args);

        std::cout << i << ": " << state << '\n';
        //std::cin.get();

        state.ip = state.registers[state.register_bound];
        ++state.ip;
    }

    std::cout << "Register 0: " << state.registers[0] << '\n';
}