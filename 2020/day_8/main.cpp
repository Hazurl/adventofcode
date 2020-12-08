#include "../common.hpp"

enum class Op {
    Acc, 
    Jmp,
    Nop,
};

struct Instruction {
    Op op;
    int arg;
};

Instruction parse_instruction(std::string const& str) {
    std::string str_op;
    Instruction instruction;
    std::stringstream(str) >> str_op >> instruction.arg;

    if (str_op == "acc") instruction.op = Op::Acc;
    else if (str_op == "jmp") instruction.op = Op::Jmp;
    else if (str_op == "nop") instruction.op = Op::Nop;
    else throw std::runtime_error("Couldn't parse instruction");

    return instruction;
}

struct Machine {
    std::size_t pointer = 0;
    int accumulator = 0;

    void execute_instruction(Instruction const& instruction) {
        switch(instruction.op) {
            case Op::Nop: break;
            case Op::Acc: accumulator += instruction.arg; break;
            case Op::Jmp: pointer += instruction.arg - 1; break;
        }

        ++pointer;
    }
};

long part_1(std::vector<std::string> const& lines) {
    std::vector<Instruction> instructions(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(instructions), parse_instruction);

    std::vector<bool> instruction_visited(instructions.size());

    Machine machine;

    while(!instruction_visited[machine.pointer]) {
        instruction_visited[machine.pointer] = true;
        machine.execute_instruction(instructions[machine.pointer]);
    }

    return machine.accumulator;
}

std::optional<Machine> is_terminating_when_changing(std::vector<Instruction> const& instructions, std::size_t instruction_to_change) {
    std::vector<bool> instruction_visited(instructions.size());

    Machine machine;

    while(machine.pointer < instructions.size() && !instruction_visited[machine.pointer]) {
        instruction_visited[machine.pointer] = true;
        Instruction ins = instructions[machine.pointer];
        
        if (machine.pointer == instruction_to_change) {
            if (ins.op == Op::Nop) {
                ins.op = Op::Jmp;
            } else {
                ins.op = Op::Nop;
            }
        }

        machine.execute_instruction(ins);
    }

    if (machine.pointer < instructions.size()) {
        return std::nullopt;
    } else {
        return machine;
    }
}

long part_2(std::vector<std::string> const& lines) {
    std::vector<Instruction> instructions(lines.size());
    std::transform(std::begin(lines), std::end(lines), std::begin(instructions), parse_instruction);

    for(std::size_t idx = 0; idx < instructions.size(); ++idx) {
        if (instructions[idx].op != Op::Acc) { // Nop or Jmp
            auto machine = is_terminating_when_changing(instructions, idx);
            if (machine) {
                return machine->accumulator;
            }
        }
    }

    return -1;
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
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