def main() -> None:
    with open("./advent_of_code/day06/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


def part1(raw_input: str) -> int:
    raw = raw_input.strip()
    for i in range(0, len(raw) - 3):
        if len(set(raw[i : i + 4])) == 4:
            return i + 4
    raise ValueError("No solution found")


def part2(raw_input: str) -> int:
    raw = raw_input.strip()
    for i in range(0, len(raw) - 13):
        if len(set(raw[i : i + 14])) == 14:
            return i + 14
    raise ValueError("No solution found")


if __name__ == "__main__":
    main()
