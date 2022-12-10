from dataclasses import dataclass


def main() -> None:
    with open("./advent_of_code/day10/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    res = "\n".join(part2(raw_input)).replace(".", " ").replace("#", "█")
    print(f"Part 2:\n{res}")


@dataclass
class Noop:
    pass


@dataclass
class AddX:
    x: int


def parse_line(line: str) -> Noop | AddX:
    if line.startswith("noop"):
        return Noop()
    elif line.startswith("addx"):
        return AddX(int(line.split()[-1]))
    else:
        raise ValueError(f"Unknown instruction: {line}")


def part1(raw_input: str) -> int:
    lines = raw_input.strip().splitlines()
    instructions = [parse_line(line) for line in lines]

    register_x = 1
    cycle_count = 0
    cycle_countdown = 20

    total_register_x = 0

    def next_cycle():
        nonlocal cycle_count
        nonlocal cycle_countdown
        nonlocal total_register_x

        cycle_count += 1
        cycle_countdown -= 1

        if cycle_countdown <= 0:
            cycle_countdown += 40
            total_register_x += register_x * cycle_count

    for instruction in instructions:
        if isinstance(instruction, Noop):
            next_cycle()
        elif isinstance(instruction, AddX):
            next_cycle()
            next_cycle()
            register_x += instruction.x
        else:
            raise ValueError(f"Unknown instruction: {instruction}")

    return total_register_x


def part2(raw_input: str) -> list[str]:
    lines = raw_input.strip().splitlines()
    instructions = [parse_line(line) for line in lines]

    register_x = 1
    cycle_count = 0

    screen = []

    def next_cycle():
        nonlocal cycle_count

        x_position = cycle_count % 40
        cycle_count += 1

        if x_position == 0:
            screen.append("")
        if x_position - 1 <= register_x <= x_position + 1:
            screen[-1] += "#"
        else:
            screen[-1] += "."

    for instruction in instructions:
        if isinstance(instruction, Noop):
            next_cycle()
        elif isinstance(instruction, AddX):
            next_cycle()
            next_cycle()
            register_x += instruction.x
        else:
            raise ValueError(f"Unknown instruction: {instruction}")

    return screen


if __name__ == "__main__":
    main()
