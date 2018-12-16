#include <iostream>
#include <array>
#include <fstream>
#include <unordered_set>
#include <limits>

using Registers = std::array<int, 4>;

struct Args {
    int a;
    int b;
    int c;
};

struct Instruction {
    int opcode;
    Args args;
};


namespace op {

    Registers addr(Registers input, Args const& args) {
        input[args.c] = input[args.a] + input[args.b];
        return input;
    }

    Registers addi(Registers input, Args const& args) {
        input[args.c] = input[args.a] + args.b;
        return input;
    }




    Registers mulr(Registers input, Args const& args) {
        input[args.c] = input[args.a] * input[args.b];
        return input;
    }

    Registers muli(Registers input, Args const& args) {
        input[args.c] = input[args.a] * args.b;
        return input;
    }




    Registers banr(Registers input, Args const& args) {
        input[args.c] = input[args.a] & input[args.b];
        return input;
    }

    Registers bani(Registers input, Args const& args) {
        input[args.c] = input[args.a] & args.b;
        return input;
    }




    Registers borr(Registers input, Args const& args) {
        input[args.c] = input[args.a] | input[args.b];
        return input;
    }

    Registers bori(Registers input, Args const& args) {
        input[args.c] = input[args.a] | args.b;
        return input;
    }




    Registers setr(Registers input, Args const& args) {
        input[args.c] = input[args.a];
        return input;
    }

    Registers seti(Registers input, Args const& args) {
        input[args.c] = args.a;
        return input;
    }




    Registers gtir(Registers input, Args const& args) {
        input[args.c] = args.a > input[args.b] ? 1 : 0;
        return input;
    }

    Registers gtri(Registers input, Args const& args) {
        input[args.c] = input[args.a] > args.b ? 1 : 0;
        return input;
    }

    Registers gtrr(Registers input, Args const& args) {
        input[args.c] = input[args.a] > input[args.b] ? 1 : 0;
        return input;
    }




    Registers eqir(Registers input, Args const& args) {
        input[args.c] = args.a == input[args.b] ? 1 : 0;
        return input;
    }

    Registers eqri(Registers input, Args const& args) {
        input[args.c] = input[args.a] == args.b ? 1 : 0;
        return input;
    }

    Registers eqrr(Registers input, Args const& args) {
        input[args.c] = input[args.a] == input[args.b] ? 1 : 0;
        return input;
    }

}

struct Sample {
    Registers before;
    Registers after;
    Instruction instruction;
};

Instruction parse_instruction(std::istream& is) {
    Instruction instruction;
    is >> instruction.opcode;
    is >> instruction.args.a;
    is >> instruction.args.b;
    is >> instruction.args.c;
    is.get();
    return instruction;
}

Sample parse_sample(std::istream& is) {
    Sample s;
    is.ignore(std::numeric_limits<std::streamsize>::max(), '[');
    for(auto& r : s.before) {
        is >> r;
        is.get();
    }

    is.get();
    s.instruction = parse_instruction(is);

    is.ignore(std::numeric_limits<std::streamsize>::max(), '[');
    for(auto& r : s.after) {
        is >> r;
        is.get();
    }
    is.get();

    return s;
}

std::ostream& operator << (std::ostream& os, Registers const& r) {
    return os << '[' << r[0] << ", " << r[1] << ", " << r[2] << ", " << r[3] << ']';
}

int main() {

    std::ifstream file("formatted_inputs/day_16");
    if (!file) {
        std::cerr << "Oops, can't open file...\n";
        return 1;
    }

    std::array<Registers(*)(Registers, Args const&), 16> ops {
        op::addr, op::addi,
        op::mulr, op::muli,
        op::banr, op::bani,
        op::borr, op::bori,
        op::setr, op::seti,
        op::gtir, op::gtri, op::gtrr,
        op::eqir, op::eqri, op::eqrr,
    };

    using op_t = Registers(*)(Registers, Args const&);

    std::array<std::unordered_set<std::size_t>, 16> impossible_op;

    int part_1_result{ 0 };
    while(!file.eof()) {
        auto s = parse_sample(file);
        int count{ 0 };

        for(std::size_t op_idx{ 0 }; op_idx < 16; ++op_idx) {
            auto res = ops[op_idx](s.before, s.instruction.args);
            impossible_op[s.instruction.opcode].insert(op_idx);
            if (res == s.after) {
                count++;

                impossible_op[s.instruction.opcode].erase(op_idx);
            }
        }

        if (count >= 3) {
            part_1_result++;
        }
    }

    std::size_t remaining{ 16 };
    std::array<op_t, 16> correct_op; correct_op.fill(nullptr);
    for(std::size_t i{ 0 }; remaining > 0; ++i) {
        if (i >= 16) i -= 16;

        if (correct_op[i] == nullptr && impossible_op[i].size() >= 15) {
            std::cout << "For #" << i << ": ";
            for(std::size_t o{ 0 }; o < 16; ++o) {
                if (impossible_op[i].count(o) == 0) {

                    for(auto& i : impossible_op) {
                        i.insert(o);
                    }

                    correct_op[i] = ops[o];

                    std::cout << o << '\n';
                    --remaining;
                    break;
                }
            }
        }
    }

    std::cout << "Part 1: " << part_1_result << '\n';

    std::ifstream file2("formatted_inputs/day_16_2");
    if (!file2) {
        std::cerr << "Oops, can't open file...\n";
        return 1;
    }

    Registers registers{ 0, 0, 0, 0 };
    while(!file2.eof()) {
        auto istr = parse_instruction(file2);
        registers = correct_op[istr.opcode](registers, istr.args);
    }

    std::cout << "Part 2: " << registers[0] << '\n';
}