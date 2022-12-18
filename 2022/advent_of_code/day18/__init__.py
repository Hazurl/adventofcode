def main() -> None:
    with open("./advent_of_code/day18/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


Pos = tuple[int, int, int]


def part1(raw_input: str) -> int:
    positions = {tuple(map(int, line.split(","))) for line in raw_input.strip().splitlines()}

    return sum(
        sum(
            1 if (x + dx, y + dy, z + dz) not in positions else 0
            for dx, dy, dz in (
                (0, 0, 1),
                (0, 0, -1),
                (0, 1, 0),
                (0, -1, 0),
                (1, 0, 0),
                (-1, 0, 0),
            )
        )
        for x, y, z in positions
    )


def part2(raw_input: str) -> int:
    positions = {tuple(map(int, line.split(","))) for line in raw_input.strip().splitlines()}
    neighbors_relative_positions = (
        (0, 0, 1),
        (0, 0, -1),
        (0, 1, 0),
        (0, -1, 0),
        (1, 0, 0),
        (-1, 0, 0),
    )

    air_neighbors = {
        (x + dx, y + dy, z + dz)
        for x, y, z in positions
        for dx, dy, dz in neighbors_relative_positions
        if (x + dx, y + dy, z + dz) not in positions
    }

    min_x = min(x for x, _, _ in positions)
    max_x = max(x for x, _, _ in positions)
    min_y = min(y for _, y, _ in positions)
    max_y = max(y for _, y, _ in positions)
    min_z = min(z for _, _, z in positions)
    max_z = max(z for _, _, z in positions)

    exterior_positions: set[Pos] = set()
    interior_positions: set[Pos] = set()

    def check_position(pos: Pos, exterior_positions: set[Pos], interior_positions: set[Pos]) -> None:
        to_check: list[Pos] = [pos]
        checked: set[Pos] = set()

        while to_check:
            p = to_check.pop()
            checked.add(p)

            if p in interior_positions:
                interior_positions |= checked
                return

            if p in exterior_positions:
                exterior_positions |= checked
                return

            if p[0] < min_x or p[0] > max_x or p[1] < min_y or p[1] > max_y or p[2] < min_z or p[2] > max_z:
                exterior_positions |= checked
                return

            for dx, dy, dz in neighbors_relative_positions:
                new_p = (p[0] + dx, p[1] + dy, p[2] + dz)
                if new_p not in positions and new_p not in checked:
                    to_check.append(new_p)

        interior_positions |= checked

    for pos in air_neighbors:
        check_position(pos, exterior_positions, interior_positions)

    return sum(
        sum(
            1 if (x + dx, y + dy, z + dz) not in positions and (x + dx, y + dy, z + dz) not in interior_positions else 0
            for dx, dy, dz in neighbors_relative_positions
        )
        for x, y, z in positions
    )


if __name__ == "__main__":
    main()
