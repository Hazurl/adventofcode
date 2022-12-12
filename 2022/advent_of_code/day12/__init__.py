from dataclasses import dataclass
from queue import PriorityQueue
from typing import Optional


def main() -> None:
    with open("./advent_of_code/day12/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


Pos = tuple[int, int]


@dataclass
class Node:
    pos: Pos
    real_cost: int
    estimated_remaining_cost: int

    @property
    def estimated_cost(self) -> int:
        return self.real_cost + self.estimated_remaining_cost

    def __lt__(self, other):
        return self.estimated_cost < other.estimated_cost


def get_neighbors(height_map: list[list[int]], pos: Pos) -> list[Pos]:
    x, y = pos
    height = height_map[y][x]

    possibles_positions = [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]
    not_out_of_bound = [(x, y) for x, y in possibles_positions if 0 <= x < len(height_map[0]) and 0 <= y < len(height_map)]
    not_too_high = [(x, y) for x, y in not_out_of_bound if height + 1 >= height_map[y][x]]

    return not_too_high


def get_reverse_neighbors(height_map: list[list[int]], pos: Pos) -> list[Pos]:
    x, y = pos
    height = height_map[y][x]

    possibles_positions = [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]
    not_out_of_bound = [(x, y) for x, y in possibles_positions if 0 <= x < len(height_map[0]) and 0 <= y < len(height_map)]
    not_too_high = [(x, y) for x, y in not_out_of_bound if height <= height_map[y][x] + 1]

    return not_too_high


def get_estimated_remaining_cost(lhs: Pos, rhs: Pos) -> int:
    return abs(lhs[0] - rhs[0]) + abs(lhs[1] - rhs[1])


def reconstruct_path(parents: dict[Pos, tuple[Optional[Pos], int]], current: Optional[Pos]) -> list[Pos]:
    path = []
    while current is not None:
        path.append(current)
        current = parents[current][0]
    return path[::-1]


def pathfinder(height_map: list[list[int]], start: Pos, goal: Pos) -> list[Pos]:
    to_check: PriorityQueue[Node] = PriorityQueue()
    to_check.put(Node(start, 0, get_estimated_remaining_cost(start, goal)))
    parents: dict[Pos, tuple[Optional[Pos], int]] = {start: (None, 0)}

    while not to_check.empty():
        current = to_check.get()
        if current.pos == goal:
            return reconstruct_path(parents, current.pos)

        for neighbor in get_neighbors(height_map, current.pos):
            neighbor_real_cost = current.real_cost + 1

            if neighbor not in parents or neighbor_real_cost < parents[neighbor][1]:
                parents[neighbor] = (current.pos, neighbor_real_cost)
                to_check.put(Node(neighbor, neighbor_real_cost, get_estimated_remaining_cost(neighbor, goal)))

    raise ValueError("No path found")


def part1(raw_input: str) -> int:
    rows = raw_input.strip().splitlines()
    height_map = [[0 if c == "S" else 26 if c == "E" else ord(c) - ord("a") for c in row] for row in rows]

    start = 0, 0
    end = 0, 0
    for y, row in enumerate(rows):
        for x, c in enumerate(row):
            if c == "S":
                start = x, y
            if c == "E":
                end = x, y

    return len(pathfinder(height_map, start, end)) - 1


def part2(raw_input: str) -> int:
    rows = raw_input.strip().splitlines()
    height_map = [[0 if c == "S" else 26 if c == "E" else ord(c) - ord("a") for c in row] for row in rows]

    end = 0, 0
    for y, row in enumerate(rows):
        for x, c in enumerate(row):
            if c == "E":
                end = x, y
                break

    costs: dict[Pos, int] = {end: 0}
    to_check = [end]
    positions_of_minimum_heights = {(x, y) for y, row in enumerate(height_map) for x, height in enumerate(row) if height == 0}

    while to_check:
        current = to_check[0]
        del to_check[0]

        for neighbor in get_reverse_neighbors(height_map, current):
            cost = costs[current] + 1
            if neighbor not in costs or cost < costs[neighbor]:
                costs[neighbor] = cost
                to_check.append(neighbor)

    return min(costs[pos] for pos in positions_of_minimum_heights if pos in costs)


if __name__ == "__main__":
    main()
