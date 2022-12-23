import re
from collections import defaultdict
from dataclasses import dataclass
from typing import Literal


def main() -> None:
    with open("./advent_of_code/day21/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


@dataclass
class Constant:
    value: int


@dataclass
class Operation:
    op: Literal["+", "-", "*", "/"]
    monkeys: tuple[str, str]

    def compute(self, monkey_results: dict[str, int]) -> int:
        a = monkey_results[self.monkeys[0]]
        b = monkey_results[self.monkeys[1]]
        return {
            "+": a + b,
            "-": a - b,
            "*": a * b,
            "/": a // b,
        }[self.op]

    def inverse_compute(self, monkey_results: dict[str, int], should_be: int) -> int:
        if self.monkeys[0] in monkey_results:
            # Find rhs such that `lhs op rhs = should_be`
            lhs = monkey_results[self.monkeys[0]]
            return {
                "+": should_be - lhs,
                "-": lhs - should_be,
                "*": should_be // lhs,
                "/": lhs // should_be,
            }[self.op]
        elif self.monkeys[1] in monkey_results:
            # Find lhs such that `lhs op rhs = should_be`
            rhs = monkey_results[self.monkeys[1]]
            return {
                "+": should_be - rhs,
                "-": should_be + rhs,
                "*": should_be // rhs,
                "/": should_be * rhs,
            }[self.op]
        else:
            raise ValueError("Both monkeys are missing")


@dataclass
class Monkey:
    name: str
    job: Constant | Operation


regex_operation = re.compile(r"([a-z]{4}) ([\+\-\*\/]) ([a-z]{4})")


def parse_monkey(line: str) -> Monkey:
    name = line[:4]
    match = regex_operation.match(line[6:])
    if match:
        op = match.group(2)
        assert op in "+-*/"
        return Monkey(name, Operation(op, (match.group(1), match.group(3))))  # type: ignore
    else:
        return Monkey(name, Constant(int(line[6:])))


def find_reverse_dependancies(monkeys: list[Monkey]) -> dict[str, set[str]]:
    tree: defaultdict[str, set[str]] = defaultdict(set)

    for monkey in monkeys:
        if isinstance(monkey.job, Operation):
            tree[monkey.job.monkeys[0]] |= {monkey.name}
            tree[monkey.job.monkeys[1]] |= {monkey.name}

    return tree


def part1(raw_input: str) -> int:
    monkeys = [parse_monkey(line) for line in raw_input.strip().strip().splitlines()]
    reverse_dependancies = find_reverse_dependancies(monkeys)
    name_to_monkey = {monkey.name: monkey for monkey in monkeys}

    monkeys_to_check = {monkey.name for monkey in monkeys if isinstance(monkey.job, Constant)}

    monkey_results = {}

    while monkeys_to_check:
        monkey_name = monkeys_to_check.pop()
        monkey = name_to_monkey[monkey_name]
        if isinstance(monkey.job, Constant):
            monkey_results[monkey_name] = monkey.job.value
            for dependant in reverse_dependancies[monkey_name]:
                monkeys_to_check.add(dependant)
        else:
            if monkey.job.monkeys[0] in monkey_results and monkey.job.monkeys[1] in monkey_results:
                monkey_results[monkey_name] = monkey.job.compute(monkey_results)
                for dependant in reverse_dependancies[monkey_name]:
                    monkeys_to_check.add(dependant)

    return monkey_results["root"]


def part2(raw_input: str) -> int:
    monkeys = [parse_monkey(line) for line in raw_input.strip().strip().splitlines()]
    reverse_dependancies = find_reverse_dependancies(monkeys)
    name_to_monkey = {monkey.name: monkey for monkey in monkeys}

    monkeys_to_check = {monkey.name for monkey in monkeys if isinstance(monkey.job, Constant)}

    monkey_results = {}

    while monkeys_to_check:
        monkey_name = monkeys_to_check.pop()
        if monkey_name == "root" or monkey_name == "humn":
            continue

        monkey = name_to_monkey[monkey_name]
        if isinstance(monkey.job, Constant):
            monkey_results[monkey_name] = monkey.job.value
            for dependant in reverse_dependancies[monkey_name]:
                monkeys_to_check.add(dependant)
        else:
            if monkey.job.monkeys[0] in monkey_results and monkey.job.monkeys[1] in monkey_results:
                monkey_results[monkey_name] = monkey.job.compute(monkey_results)
                for dependant in reverse_dependancies[monkey_name]:
                    monkeys_to_check.add(dependant)

    should_result_in = next(monkey_results[m] for m in name_to_monkey["root"].job.monkeys if m in monkey_results)  # type: ignore
    monkey_name_that_should_be_result = next(m for m in name_to_monkey["root"].job.monkeys if m not in monkey_results)  # type: ignore

    while monkey_name_that_should_be_result != "humn":
        monkey = name_to_monkey[monkey_name_that_should_be_result]
        should_result_in = monkey.job.inverse_compute(monkey_results, should_result_in)  # type: ignore
        monkey_name_that_should_be_result = next(m for m in monkey.job.monkeys if m not in monkey_results)  # type: ignore

    return should_result_in


if __name__ == "__main__":
    main()
