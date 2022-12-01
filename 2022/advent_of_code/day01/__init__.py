import itertools as it


def main() -> None:
    with open("./advent_of_code/day01/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


def part1(raw_input: str) -> int:
    lines = raw_input.splitlines()
    groups = [map(int, list(g)) for k, g in it.groupby(lines, lambda x: x.strip() != "") if k]
    return max(sum(g) for g in groups)


def part2(raw_input: str) -> int:
    lines = raw_input.splitlines()
    groups = [map(int, list(g)) for k, g in it.groupby(lines, lambda x: x.strip() != "") if k]
    calories = sorted(sum(g) for g in groups)
    return sum(calories[-3:])


if __name__ == "__main__":
    main()
