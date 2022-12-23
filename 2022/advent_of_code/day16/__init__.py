import re
from dataclasses import dataclass
from pprint import pprint
from typing import Generator, Iterator, Optional


def main() -> None:
    with open("./advent_of_code/day16/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


regex = re.compile(r"Valve (?P<name>\w+) has flow rate=(?P<flow_rate>\d+); tunnels? leads? to valves? (?P<connections>\w+(, \w+)*)")


@dataclass
class Valve:
    name: str
    flow_rate: int
    connections: list[str]


@dataclass
class State:
    current: Valve
    minute_cost: int
    valves_opened: set[str]
    benefit: int


@dataclass
class MoveToAndOpen:
    valve: Valve
    minute_cost: int

    def benefit(self, remaining_minutes: int) -> int:
        return max(0, self.valve.flow_rate * (remaining_minutes - self.minute_cost))

    def average_benefit(self, remaining_minutes: int) -> float:
        return max(0, self.valve.flow_rate * (remaining_minutes - self.minute_cost)) / self.minute_cost

    def __repr__(self) -> str:
        return f"{self.valve.name} in {self.minute_cost} minutes"


@dataclass
class Wait:
    valve: Valve

    def benefit(self, _: int) -> int:
        return 0


def parse_valve(string: str) -> Valve:
    match = regex.match(string)
    if not match:
        raise ValueError(f"Could not parse string: {string}")

    return Valve(
        name=match.group("name"),
        flow_rate=int(match.group("flow_rate")),
        connections=match.group("connections").split(", "),
    )


def get_all_possible_actions_with_cost(valve: Valve, valves: dict[str, Valve]) -> list[MoveToAndOpen]:
    valves_done = {valve.name}
    actions = []

    current_valves = [valve]
    next_valves: list[Valve] = []
    minute_cost = 0

    while current_valves:
        for v in current_valves:
            if v.flow_rate > 0 and v.name != valve.name:
                actions.append(MoveToAndOpen(v, minute_cost + 1))  # + opening_cost

            for connection in v.connections:
                if connection not in valves_done:
                    next_valves.append(valves[connection])
                    valves_done.add(connection)

        minute_cost += 1
        current_valves = next_valves
        next_valves = []

        if minute_cost >= 30:
            break

    return actions


def part1(raw_input: str) -> int:
    valves_list = [parse_valve(line) for line in raw_input.strip().splitlines()]
    valves = {valve.name: valve for valve in valves_list}

    valves_actions = {valve.name: get_all_possible_actions_with_cost(valve, valves) for valve in valves_list}

    valves_with_some_flow_rate = {valve.name for valve in valves_list if valve.flow_rate > 0}
    assert "AA" not in valves_with_some_flow_rate

    def compute_best_flow_rate(last_valve: Valve, remaining_valves: set[str], remaining_minute: int) -> tuple[int, list[str]]:
        if not remaining_valves:
            return 0, []

        if remaining_minute <= 0:
            return 0, []

        best_benefit = 0
        nn = []

        for action in valves_actions[last_valve.name]:
            if action.valve.name in remaining_valves:
                new_remaining_valves = remaining_valves.copy()
                new_remaining_valves.remove(action.valve.name)

                action_benefit = action.benefit(remaining_minute)
                if action_benefit <= 0:
                    continue

                benefit, n = compute_best_flow_rate(
                    action.valve,
                    new_remaining_valves,
                    remaining_minute - action.minute_cost,
                )
                benefit += action_benefit

                if benefit > best_benefit:
                    best_benefit = benefit
                    nn = [action.valve.name] + n

        return best_benefit, nn

    best_flow, n = compute_best_flow_rate(valves["AA"], valves_with_some_flow_rate, 30)

    print(["AA"] + n)
    return best_flow


@dataclass
class Path:
    current_valve: str
    actions: list[MoveToAndOpen]
    benefit: int
    cost: int

    def can_add_action(self, action: MoveToAndOpen, max_cost: int) -> bool:
        return self.cost + action.minute_cost <= max_cost

    def with_action(self, action: MoveToAndOpen, max_cost: int) -> "Path":
        return Path(
            current_valve=action.valve.name,
            actions=self.actions + [action],
            benefit=self.benefit + action.benefit(max_cost - self.cost),
            cost=self.cost + action.minute_cost,
        )

    def __repr__(self) -> str:
        return "<<" + " -> ".join([action.valve.name for action in self.actions]) + ">>"


def get_all_sets_of_paths(valves: list[str], /, force_all) -> Generator[tuple[set[str], set[str]], None, None]:
    if not valves:
        yield set(), set()
        return

    valve = valves.pop()
    for left, right in get_all_sets_of_paths(valves, force_all=force_all):
        yield left | {valve}, right
        yield left, right | {valve}

    if not force_all:
        yield from get_all_sets_of_paths(valves, force_all=force_all)


def compute_best_benefit_for_path_with_valves(
    start_valve: Valve, valves: set[str], all_actions: dict[tuple[str, str], MoveToAndOpen], max_cost: int
) -> Optional[int]:
    # print(f"Compute {valves}")
    benefit = 0
    cost_remaining = max_cost
    current_valve = start_valve.name

    while valves:
        best_next_valve = max(valves, key=lambda v: all_actions[(current_valve, v)].average_benefit(cost_remaining))
        valves.remove(best_next_valve)
        action = all_actions[(current_valve, best_next_valve)]
        benefit += action.benefit(cost_remaining)
        cost_remaining -= action.minute_cost
        # print(f" -> {action.valve.name} ({cost_remaining}, {benefit})")
        current_valve = action.valve.name

    return benefit if cost_remaining >= 0 else None


def set_to_hashable(s: set[str]) -> tuple[str, ...]:
    return tuple(sorted(s))


def part2x(raw_input: str) -> int:
    valves_list = [parse_valve(line) for line in raw_input.strip().splitlines()]
    valves = {valve.name: valve for valve in valves_list}

    all_actions = {
        (valve.name, action.valve.name): action for valve in valves_list for action in get_all_possible_actions_with_cost(valve, valves)
    }

    valves_with_some_flow_rate = [valve.name for valve in valves_list if valve.flow_rate > 0]
    assert "AA" not in valves_with_some_flow_rate
    start_valve = valves["AA"]
    max_cost = 26

    best_path_cache: dict[tuple[str, tuple[str, ...]], Path] = dict()

    def cached(func):
        def wrapper(starting_with: str, valves: set[str]) -> Optional[Path]:
            hashed = (starting_with, set_to_hashable(valves))
            if hashed in best_path_cache:
                return best_path_cache[hashed]

            result = func(starting_with, valves)
            best_path_cache[hashed] = result
            return result

        return wrapper

    @cached
    def get_best_path_with_valves(starting_with: str, valves: set[str]) -> Optional[Path]:
        if not valves:
            return Path(starting_with, [], 0, 0)

        if len(valves) == 1:
            valve = valves.pop()
            action = all_actions[(starting_with, valve)]
            return Path(valve, [action], action.benefit(max_cost), action.minute_cost)

        best_path_with_valves = None
        best_benefit = 0
        for a, b in get_all_sets_of_paths(list(valves), force_all=True):
            if not a or not b:
                continue

            def get_best_combined_path(a: set[str], b: set[str]) -> Optional[Path]:
                bpa = get_best_path_with_valves(starting_with, a)
                if bpa is None:
                    return None

                bpb = get_best_path_with_valves(bpa.current_valve, b)
                if bpb is None:
                    return None

                combined_path = bpa
                for action in bpb.actions:
                    combined_path = combined_path.with_action(action, max_cost)

                return combined_path

            combined_path_ab = get_best_combined_path(a, b)
            combined_path_ba = get_best_combined_path(b, a)

            if combined_path_ab is not None and combined_path_ab.cost <= max_cost and combined_path_ab.benefit > best_benefit:
                print(f"New best: ({combined_path_ab.benefit}) {combined_path_ab}")
                best_benefit = combined_path_ab.benefit
                best_path_with_valves = combined_path_ab

            if combined_path_ba is not None and combined_path_ba.cost <= max_cost and combined_path_ba.benefit > best_benefit:
                print(f"New best: ({combined_path_ba.benefit}) {combined_path_ba}")
                best_benefit = combined_path_ba.benefit
                best_path_with_valves = combined_path_ba

        return best_path_with_valves

    best_benefit = 0

    for a, b in get_all_sets_of_paths(valves_with_some_flow_rate, force_all=False):
        pa = get_best_path_with_valves(start_valve.name, a)
        pb = get_best_path_with_valves(start_valve.name, b)

        if pa is None or pb is None:
            continue

        benefit = pa.benefit + pb.benefit
        if benefit > best_benefit:
            print(f"###\nNew best benefit: {benefit} ({pa} // {pb})")
            best_benefit = benefit

    pprint(best_path_cache)

    return best_benefit


@dataclass
class SetOfSet:
    sets: list[set[str]]
    sets_hashed: set[tuple[str, ...]]

    def add(self, s: set[str]) -> None:
        hashed = set_to_hashable(s)
        if hashed not in self.sets_hashed:
            self.sets.append(s)
            self.sets_hashed.add(hashed)

    def remove(self, s: set[str]) -> None:
        hashed = set_to_hashable(s)
        if hashed in self.sets_hashed:
            self.sets.remove(s)
            self.sets_hashed.remove(hashed)

    def remove_all(self, sos: "SetOfSet") -> None:
        for s in sos:
            self.remove(s)

    def __contains__(self, s: set[str]) -> bool:
        return set_to_hashable(s) in self.sets_hashed

    def __init__(self) -> None:
        self.sets = []
        self.sets_hashed = set()

    def __len__(self) -> int:
        return len(self.sets)

    def __iter__(self) -> Iterator[set[str]]:
        return iter(self.sets)

    def is_empty(self) -> bool:
        return not self.sets

    @staticmethod
    def from_sets(sets: list[set[str]]) -> "SetOfSet":
        sos = SetOfSet()
        for s in sets:
            sos.add(s)

        return sos

    def __repr__(self) -> str:
        return f"{self.sets}"


@dataclass
class VisitorSet:
    common: set[str]
    childrens: list[set[str]]

    def visit(self, valve: str) -> "VisitorSet":
        if valve in self.common:
            return VisitorSet(set(), [])

        return VisitorSet(self.common | {valve}, [children for children in self.childrens if valve not in children])

    def is_empty(self) -> bool:
        return not self.common

    def merge(self, other: "VisitorSet") -> "VisitorSet":
        if self.is_empty():
            return other

        if other.is_empty():
            return self

        new_visitor_set = VisitorSet(self.common & other.common, [])

        for children in self.childrens:
            new_visitor_set.childrens.append((children | self.common) - new_visitor_set.common)
        if self.common - new_visitor_set.common:
            new_visitor_set.childrens.append(self.common - new_visitor_set.common)

        for children in other.childrens:
            new_visitor_set.childrens.append((children | other.common) - new_visitor_set.common)
        if other.common - new_visitor_set.common:
            new_visitor_set.childrens.append(other.common - new_visitor_set.common)

        return new_visitor_set

    def __repr__(self) -> str:
        if self.is_empty():
            return "VD(empty)"
        else:
            return f"VS({self.common}, {self.childrens})"


def part2y(raw_input: str) -> int:
    valves_list = [parse_valve(line) for line in raw_input.strip().splitlines()]
    valves = {valve.name: valve for valve in valves_list}

    actions_for_valve = {valve.name: get_all_possible_actions_with_cost(valve, valves) for valve in valves_list}
    all_actions = {
        (valve.name, action.valve.name): action for valve in valves_list for action in get_all_possible_actions_with_cost(valve, valves)
    }

    valves_with_some_flow_rate = [valve.name for valve in valves_list if valve.flow_rate > 0]
    assert "AA" not in valves_with_some_flow_rate
    start_valve = valves["AA"]
    valves_with_some_flow_rate.append(start_valve.name)
    max_minute = 30

    states = [{valve: SetOfSet() for valve in valves_with_some_flow_rate} for _ in range(max_minute)]

    states[0][start_valve.name] = SetOfSet.from_sets([{start_valve.name}])

    for minute in range(max_minute):
        state = states[minute]
        for valve, set_of_sets in state.items():
            if set_of_sets.is_empty():
                continue

            paths_with_any_action = SetOfSet()

            for action in actions_for_valve[valve]:
                destination = action.valve.name
                target_minute = minute + action.minute_cost
                if target_minute >= max_minute:
                    continue

                for s in set_of_sets:
                    if destination in s:
                        continue

                    paths_with_any_action.add(s)
                    states[target_minute][destination].add(s | {destination})

            set_of_sets.remove_all(paths_with_any_action)

    pprint(states)

    best_benefit = 0
    for minute, state in enumerate(states):
        for valve, sos in state.items():
            for s in sos:
                benefit = compute_best_benefit_for_path_with_valves(start_valve, s - {start_valve.name}, all_actions, max_minute)
                if benefit and benefit > best_benefit:
                    print(f"New best benefit: {benefit} ({minute}, {valve}, {s})")
                    best_benefit = benefit

    return best_benefit


def foo(
    flow_rate: int,
    current_pressure: int,
    next_action_minute: tuple[int, int],
    next_valve: tuple[str, str],
    minute: int,
    max_minute: int,
    actions_for_valve: dict[str, list[MoveToAndOpen]],
    valves_opened: set[str],
) -> Generator[int, None, None]:

    if minute >= max_minute:
        yield current_pressure
        return

    if minute == next_action_minute[0] and minute == next_action_minute[1]:
        for a_action in actions_for_valve[next_valve[0]]:
            if a_action.valve.name in valves_opened:
                continue

            for b_action in actions_for_valve[next_valve[1]]:
                if b_action.valve.name in valves_opened:
                    continue

                if a_action.valve.name == b_action.valve.name:
                    continue

                next_minute = min(minute + a_action.minute_cost, minute + b_action.minute_cost, max_minute)

                yield from foo(
                    flow_rate + a_action.valve.flow_rate + b_action.valve.flow_rate,
                    current_pressure + flow_rate * (next_minute - minute),
                    (minute + a_action.minute_cost, minute + b_action.minute_cost),
                    (a_action.valve.name, b_action.valve.name),
                    next_minute,
                    max_minute,
                    actions_for_valve,
                    valves_opened | {a_action.valve.name, b_action.valve.name},
                )

    elif minute == next_action_minute[0]:
        for action in actions_for_valve[next_valve[0]]:
            if action.valve.name in valves_opened:
                continue

            next_minute = min(minute + action.minute_cost, next_action_minute[1], max_minute)

            yield from foo(
                flow_rate + action.valve.flow_rate,
                current_pressure * (next_minute - minute),
                (minute + action.minute_cost, next_action_minute[1]),
                (action.valve.name, next_valve[1]),
                next_minute,
                max_minute,
                actions_for_valve,
                valves_opened | {action.valve.name},
            )

    elif minute == next_action_minute[1]:
        for action in actions_for_valve[next_valve[1]]:
            if action.valve.name in valves_opened:
                continue

            next_minute = min(minute + action.minute_cost, next_action_minute[0], max_minute)

            yield from foo(
                flow_rate + action.valve.flow_rate,
                current_pressure + flow_rate * (next_minute - minute),
                (next_action_minute[0], minute + action.minute_cost),
                (next_valve[0], action.valve.name),
                next_minute,
                max_minute,
                actions_for_valve,
                valves_opened | {action.valve.name},
            )
    else:
        assert False

    yield current_pressure + flow_rate * (max_minute - minute)


def part2(raw_input: str) -> int:
    # Not working...
    valves_list = [parse_valve(line) for line in raw_input.strip().splitlines()]
    valves = {valve.name: valve for valve in valves_list}

    actions_for_valve = {valve.name: get_all_possible_actions_with_cost(valve, valves) for valve in valves_list}
    valves_with_some_flow_rate = [valve.name for valve in valves_list if valve.flow_rate > 0]
    assert "AA" not in valves_with_some_flow_rate
    start_valve = valves["AA"]
    valves_with_some_flow_rate.append(start_valve.name)
    max_minute = 30

    best_benefit = 0
    for p in foo(0, 0, (0, 0), ("AA", "AA"), 0, max_minute, actions_for_valve, set()):
        if p > best_benefit:
            best_benefit = p

    return best_benefit


if __name__ == "__main__":
    main()
