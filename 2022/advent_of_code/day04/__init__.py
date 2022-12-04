def main() -> None:
    with open("./advent_of_code/day04/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


def part1(raw_input: str) -> int:
    lines = [line.strip().split(",") for line in raw_input.splitlines() if line.strip()]
    ranges = [(tuple(map(int, lhs.split("-"))), tuple(map(int, rhs.split("-")))) for lhs, rhs in lines]

    return sum(1 for ((a, b), (c, d)) in ranges if (a <= c and d <= b) or (c <= a and b <= d))


def part2(raw_input: str) -> int:
    lines = [line.strip().split(",") for line in raw_input.splitlines() if line.strip()]
    ranges = [(tuple(map(int, lhs.split("-"))), tuple(map(int, rhs.split("-")))) for lhs, rhs in lines]

    return sum(1 for ((a, b), (c, d)) in ranges if not (c > b or d < a))


if __name__ == "__main__":
    main()
