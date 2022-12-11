import re
from dataclasses import dataclass
from enum import Enum
from functools import reduce
from typing import Optional


def main() -> None:
    with open("./advent_of_code/day11/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


@dataclass
class Operation:
    class Op(Enum):
        ADD = "ADD"
        MUL = "MUL"

    lhs: Optional[int]  # None means old
    rhs: Optional[int]  # None means old
    op: Op

    def apply(self, old: int) -> int:
        lhs = self.lhs if self.lhs is not None else old
        rhs = self.rhs if self.rhs is not None else old
        return lhs + rhs if self.op == self.Op.ADD else lhs * rhs


@dataclass
class Monkey:
    items: list[int]
    operation: Operation
    test_divisible_by: int
    if_true_monkey_index: int
    if_false_monkey_index: int

    def next_monkey_index(self, value: int) -> int:
        return self.if_true_monkey_index if value % self.test_divisible_by == 0 else self.if_false_monkey_index


operation_regex = re.compile(r"new = (\d+|old) ([+*]) (\d+|old)")


def parse_operation(operation: str) -> Operation:
    match = operation_regex.match(operation)
    if match is None:
        raise ValueError(f"Could not parse operation: {operation}")

    lhs, op, rhs = match.groups()

    return Operation(
        lhs=int(lhs) if lhs != "old" else None,
        rhs=int(rhs) if rhs != "old" else None,
        op=Operation.Op.ADD if op == "+" else Operation.Op.MUL,
    )


monkey_regex = re.compile(
    r"Monkey (\d+):\n  Starting items: (.+)\n  Operation: (.+)\n  Test: divisible by (\d+)\n    If true: throw to monkey (\d+)\n    If false: throw to monkey (\d+)"  # noqa
)


def parse_monkey(monkey: str) -> Monkey:
    match = monkey_regex.match(monkey)
    if match is None:
        raise ValueError(f"Could not parse monkey: {monkey}")

    _, items, operation, test_divisible_by, if_true_monkey_index, if_false_monkey_index = match.groups()

    return Monkey(
        items=[int(item) for item in items.split(", ")],
        operation=parse_operation(operation),
        test_divisible_by=int(test_divisible_by),
        if_true_monkey_index=int(if_true_monkey_index),
        if_false_monkey_index=int(if_false_monkey_index),
    )


def part1(raw_input: str) -> int:
    monkeys = [parse_monkey(monkey) for monkey in raw_input.split("\n\n")]
    monkeys_inspect_count = [0] * len(monkeys)

    for _ in range(20):
        for monkey_index, monkey in enumerate(monkeys):
            items = monkey.items
            monkey.items = []

            monkeys_inspect_count[monkey_index] += len(items)

            for item in items:
                worry = monkey.operation.apply(item) // 3
                next_monkey_index = monkey.next_monkey_index(worry)
                monkeys[next_monkey_index].items.append(worry)
                assert next_monkey_index != monkey_index

    monkeys_inspect_count.sort()
    return monkeys_inspect_count[-1] * monkeys_inspect_count[-2]


def part2(raw_input: str) -> int:
    monkeys = [parse_monkey(monkey) for monkey in raw_input.split("\n\n")]
    monkeys_inspect_count = [0] * len(monkeys)
    max_mul = reduce(lambda acc, monkey: acc * monkey.test_divisible_by, monkeys, 1)

    for _ in range(10000):
        for monkey_index, monkey in enumerate(monkeys):
            items = monkey.items
            monkey.items = []

            monkeys_inspect_count[monkey_index] += len(items)

            for item in items:
                worry = monkey.operation.apply(item)
                next_monkey_index = monkey.next_monkey_index(worry)
                monkeys[next_monkey_index].items.append(worry % max_mul)
                assert next_monkey_index != monkey_index

    monkeys_inspect_count.sort()
    return monkeys_inspect_count[-1] * monkeys_inspect_count[-2]


if __name__ == "__main__":
    main()
