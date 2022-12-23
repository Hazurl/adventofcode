from dataclasses import dataclass
from enum import Enum


def main() -> None:
    with open("./advent_of_code/day20/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


@dataclass(frozen=True)
class MixedValue:
    ith: int
    value: int


def part1(raw_input: str) -> int:
    mixed_values = [MixedValue(i, int(line)) for i, line in enumerate(raw_input.strip().splitlines())]
    ith_to_index = {v.ith: i for i, v in enumerate(mixed_values)}

    for i in range(len(mixed_values)):
        index = ith_to_index[i]
        value = mixed_values[index].value
        rng = range(1, value + 1) if value > 0 else range(-1, value - 1, -1)

        prev_d_index = 0
        for d_index in rng:
            index_a = (index + d_index + len(mixed_values)) % len(mixed_values)
            index_b = (index + prev_d_index + len(mixed_values)) % len(mixed_values)
            mixed_values[index_a], mixed_values[index_b] = (
                mixed_values[index_b],
                mixed_values[index_a],
            )

            ith_to_index[mixed_values[index_a].ith] = index_a
            ith_to_index[mixed_values[index_b].ith] = index_b

            prev_d_index = d_index

    index_of_0 = next(i for i, mv in enumerate(mixed_values) if mv.value == 0)
    return (
        mixed_values[(index_of_0 + 1000) % len(mixed_values)].value
        + mixed_values[(index_of_0 + 2000) % len(mixed_values)].value
        + mixed_values[(index_of_0 + 3000) % len(mixed_values)].value
    )


@dataclass
class Node:
    id: int
    next_node: "Node"
    prev_node: "Node"
    value: int

    def to_list(self) -> list[int]:
        lst = [self.value]
        node = self.next_node
        while node.id != self.id:
            lst.append(node.value)
            node = node.next_node
        return lst

    def __repr__(self) -> str:
        return f"Node(#{self.id}, {self.value})"


class Direction(Enum):
    NEXT = "next_node"
    PREV = "prev_node"


def swap_nodes(node: Node, direction: Direction) -> None:
    # GIVEN a <-> b <-> c <-> d
    # SWAP b <-> c
    if direction == Direction.NEXT:
        # WHERE node is b
        a = node.prev_node
        b = node
        c = node.next_node
        d = node.next_node.next_node
        a.next_node = c
        c.prev_node = a
        c.next_node = b
        b.prev_node = c
        b.next_node = d
        d.prev_node = b
    elif direction == Direction.PREV:
        # WHERE node is c
        a = node.prev_node.prev_node
        b = node.prev_node
        c = node
        d = node.next_node
        a.next_node = c
        c.prev_node = a
        c.next_node = b
        b.prev_node = c
        b.next_node = d
        d.prev_node = b


def part2(raw_input: str) -> int:
    ordered_nodes = [Node(i, None, None, int(line) * 811589153) for i, line in enumerate(raw_input.strip().splitlines())]  # type: ignore
    node_0 = next(node for node in ordered_nodes if node.value == 0)
    for i, node in enumerate(ordered_nodes):
        node.next_node = ordered_nodes[(i + 1) % len(ordered_nodes)]
        node.prev_node = ordered_nodes[(i - 1) % len(ordered_nodes)]

    for r in range(10):
        for node in ordered_nodes:
            move_amount = abs(node.value) % (len(ordered_nodes) - 1)
            direction = Direction.NEXT if node.value > 0 else Direction.PREV

            for _ in range(move_amount):
                swap_nodes(node, direction)

    nodes = node_0.to_list()
    return nodes[1000 % len(nodes)] + nodes[2000 % len(nodes)] + nodes[3000 % len(nodes)]


if __name__ == "__main__":
    main()
