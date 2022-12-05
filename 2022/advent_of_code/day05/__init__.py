import re
from dataclasses import dataclass


def main() -> None:
    with open("./advent_of_code/day05/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


@dataclass
class Instruction:
    from_stack: int
    to_stack: int
    count: int


def parse_instructions(raw_instructions: str) -> list[Instruction]:
    # in the format 'move <count> from <from_stack> to <to_stack>'
    compiled_re = re.compile(r"move (?P<count>\d+) from (?P<from_stack>\d+) to (?P<to_stack>\d+)")
    instructions = []
    for line in raw_instructions.splitlines():
        line = line.strip()
        if not line:
            continue

        match = compiled_re.match(line)
        if match:
            instructions.append(
                Instruction(
                    from_stack=int(match.group("from_stack")),
                    to_stack=int(match.group("to_stack")),
                    count=int(match.group("count")),
                )
            )
        else:
            raise ValueError(f"Could not parse instruction: {line}")

    return instructions


def parse_stacks(raw_stacks: str) -> list[list[str]]:
    stacks: list[list[str]] = []
    for row in raw_stacks.splitlines()[:-1]:
        crates = [row[i : i + 4].strip().replace("[", "").replace("]", "") for i in range(0, len(row), 4)]
        for index, crate in enumerate(crates):
            if len(stacks) <= index:
                stacks.append([])
            stacks[index].append(crate)

    return [[crate for crate in reversed(stack) if crate] for stack in stacks]


def part1(raw_input: str) -> str:
    raw_stacks, raw_instructions = raw_input.split("\n\n")
    instructions = parse_instructions(raw_instructions)
    stacks = parse_stacks(raw_stacks)

    for instruction in instructions:
        crates = stacks[instruction.from_stack - 1][-instruction.count :]
        stacks[instruction.from_stack - 1] = stacks[instruction.from_stack - 1][: -instruction.count]
        stacks[instruction.to_stack - 1].extend(reversed(crates))

    return "".join(stack[-1] for stack in stacks)


def part2(raw_input: str) -> str:
    raw_stacks, raw_instructions = raw_input.split("\n\n")
    instructions = parse_instructions(raw_instructions)
    stacks = parse_stacks(raw_stacks)

    for instruction in instructions:
        crates = stacks[instruction.from_stack - 1][-instruction.count :]
        stacks[instruction.from_stack - 1] = stacks[instruction.from_stack - 1][: -instruction.count]
        stacks[instruction.to_stack - 1].extend(crates)

    return "".join(stack[-1] for stack in stacks)


if __name__ == "__main__":
    main()
