import functools
import json
from enum import Enum
from typing import Union


def main() -> None:
    with open("./advent_of_code/day13/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


Value = Union[int, list["Value"]]


def to_str(value: Value) -> str:
    if isinstance(value, list):
        return "[" + ",".join(to_str(sub_value) for sub_value in value) + "]"
    else:
        return str(value)


class Comp(Enum):
    LESS_THAN = 0
    EQUAL = 1
    GREATER_THAN = 2

    def to_int(self) -> int:
        if self == Comp.LESS_THAN:
            return -1
        elif self == Comp.EQUAL:
            return 0
        else:
            return 1


def compare_int(lhs: int, rhs: int) -> Comp:
    if lhs < rhs:
        return Comp.LESS_THAN
    elif lhs == rhs:
        return Comp.EQUAL
    else:
        return Comp.GREATER_THAN


def compare(lhs: Value, rhs: Value) -> Comp:
    if isinstance(lhs, int) and isinstance(rhs, int):
        return compare_int(lhs, rhs)
    elif isinstance(lhs, int):
        return compare([lhs], rhs)
    elif isinstance(rhs, int):
        return compare(lhs, [rhs])
    else:
        for index in range(max(len(lhs), len(rhs))):
            if index >= len(lhs) and index >= len(rhs):
                return Comp.EQUAL
            elif index >= len(lhs):
                return Comp.LESS_THAN
            elif index >= len(rhs):
                return Comp.GREATER_THAN

            comp = compare(lhs[index], rhs[index])
            if comp != Comp.EQUAL:
                return comp

        return Comp.EQUAL


def part1(raw_input: str) -> int:
    cases = [(json.loads(line) for line in both_lines.split("\n")) for both_lines in raw_input.strip().split("\n\n")]
    return sum(index + 1 for index, (lhs, rhs) in enumerate(cases) if compare(lhs, rhs) == Comp.LESS_THAN)


def part2(raw_input: str) -> int:
    lines = [json.loads(line) for line in raw_input.strip().split("\n") if line.strip()]
    lines.append([[2]])
    lines.append([[6]])

    def compare_to_int(lhs: Value, rhs: Value) -> int:
        return compare(lhs, rhs).to_int()

    lines.sort(key=functools.cmp_to_key(compare_to_int))

    return (lines.index([[2]]) + 1) * (lines.index([[6]]) + 1)


if __name__ == "__main__":
    main()
