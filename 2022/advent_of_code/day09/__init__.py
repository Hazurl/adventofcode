from enum import Enum


def main() -> None:
    with open("./advent_of_code/day09/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


class Direction(Enum):
    U = "U"
    D = "D"
    L = "L"
    R = "R"


Position = tuple[int, int]


def next_tail_position(head: Position, tail: Position) -> Position:
    hx, hy = head
    tx, ty = tail

    if abs(hx - tx) <= 1 and abs(hy - ty) <= 1:
        return tail

    if hx == tx:
        if hy > ty:
            return tx, ty + 1
        else:
            return tx, ty - 1

    if hy == ty:
        if hx > tx:
            return tx + 1, ty
        else:
            return tx - 1, ty

    if hx > tx:
        if hy > ty:
            return tx + 1, ty + 1
        else:
            return tx + 1, ty - 1
    else:
        if hy > ty:
            return tx - 1, ty + 1
        else:
            return tx - 1, ty - 1


def move(pos: Position, direction: Direction) -> Position:
    x, y = pos

    if direction == Direction.U:
        return x, y + 1
    elif direction == Direction.D:
        return x, y - 1
    elif direction == Direction.L:
        return x - 1, y
    elif direction == Direction.R:
        return x + 1, y
    else:
        raise ValueError(f"Unknown direction: {direction}")


def part1(raw_input: str) -> int:
    instructions = [
        (Direction[direction], int(amount)) for direction, amount in [line.split(" ") for line in raw_input.strip().splitlines()]
    ]

    head = (0, 0)
    tail = (0, 0)

    tail_positions = {tail}

    for direction, amount in instructions:
        for _ in range(amount):
            head = move(head, direction)
            tail = next_tail_position(head, tail)
            tail_positions.add(tail)

    return len(tail_positions)


def part2(raw_input: str) -> int:
    instructions = [
        (Direction[direction], int(amount)) for direction, amount in [line.split(" ") for line in raw_input.strip().splitlines()]
    ]

    head = (0, 0)
    tails = [(0, 0) for _ in range(9)]

    tail_positions = set(tails)

    for direction, amount in instructions:
        for _ in range(amount):
            head = move(head, direction)
            for i in range(9):
                previous_tail = tails[i - 1] if i > 0 else head
                tails[i] = next_tail_position(previous_tail, tails[i])
            tail_positions.add(tails[-1])

    return len(tail_positions)


if __name__ == "__main__":
    main()
