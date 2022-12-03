from typing import Iterable


def main() -> None:
    with open("./advent_of_code/day03/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


def compute_score(value: Iterable[str]) -> int:
    return sum((ord(char) - ord("a") + 1 if char.islower() else ord(char) - ord("A") + 27) for char in value)


def part1(raw_input: str) -> int:
    lines = [line.strip() for line in raw_input.splitlines() if line.strip()]
    rucksacks = [(line[: len(line) // 2], line[len(line) // 2 :]) for line in lines]
    return sum(compute_score(set(rucksack[0]) & set(rucksack[1])) for rucksack in rucksacks)


def part2(raw_input: str) -> int:
    lines = [line.strip() for line in raw_input.splitlines() if line.strip()]
    rucksacks = [lines[i : i + 3] for i in range(0, len(lines), 3)]

    return sum(compute_score(set(rucksack[0]) & set(rucksack[1]) & set(rucksack[2])) for rucksack in rucksacks)


if __name__ == "__main__":
    main()
