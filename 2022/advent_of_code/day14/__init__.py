from enum import Enum


def main() -> None:
    with open("./advent_of_code/day14/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


Pos = tuple[int, int]


class Type(Enum):
    EMPTY = 0
    ROCK = 1
    SAND = 2


def parse_rock_path(line: str) -> list[Pos]:
    raw_positions = line.split(" -> ")
    positions = []
    for raw_pos in raw_positions:
        a, b = raw_pos.split(",")
        positions.append((int(a), int(b)))
    return positions


def part1(raw_input: str) -> int:
    rock_paths = [parse_rock_path(line) for line in raw_input.strip().splitlines()]

    min_pos, max_pos = (0, 0), (0, 0)

    for rock_path in rock_paths:
        for pos in rock_path:
            min_pos = (min(min_pos[0], pos[0]), min(min_pos[1], pos[1]))
            max_pos = (max(max_pos[0], pos[0]), max(max_pos[1], pos[1]))

    for rock_path in rock_paths:
        for i in range(len(rock_path)):
            rock_path[i] = (rock_path[i][0] - min_pos[0], rock_path[i][1] - min_pos[1])

    grid: list[list[Type]] = []

    def set_grid(x: int, y: int, ty: Type) -> None:
        while len(grid) <= y:
            grid.append([])

        while len(grid[y]) <= x:
            grid[y].append(Type.EMPTY)

        grid[y][x] = ty

    def get_grid(x: int, y: int) -> Type:
        if y < 0 or y >= len(grid):
            return Type.EMPTY
        elif x < 0 or x >= len(grid[y]):
            return Type.EMPTY
        else:
            return grid[y][x]

    for rock_path in rock_paths:
        for a, b in zip(rock_path, rock_path[1:]):
            if a[0] == b[0]:
                for y in range(min(a[1], b[1]), max(a[1], b[1]) + 1):
                    set_grid(a[0], y, Type.ROCK)
            else:
                for x in range(min(a[0], b[0]), max(a[0], b[0]) + 1):
                    set_grid(x, a[1], Type.ROCK)

    sand_origin = (500 - min_pos[0], -min_pos[1])

    count_sand_at_rest = 0

    while True:
        current_sand_position = sand_origin
        while True:
            if current_sand_position[1] >= max_pos[1]:
                return count_sand_at_rest

            if get_grid(current_sand_position[0], current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0], current_sand_position[1] + 1)
            elif get_grid(current_sand_position[0] - 1, current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0] - 1, current_sand_position[1] + 1)
            elif get_grid(current_sand_position[0] + 1, current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0] + 1, current_sand_position[1] + 1)
            else:
                set_grid(current_sand_position[0], current_sand_position[1], Type.SAND)
                count_sand_at_rest += 1
                break


def part2(raw_input: str) -> int:
    rock_paths = [parse_rock_path(line) for line in raw_input.strip().splitlines()]

    min_pos, max_pos = (0, 0), (0, 0)

    for rock_path in rock_paths:
        for pos in rock_path:
            min_pos = (min(min_pos[0], pos[0]), min(min_pos[1], pos[1]))
            max_pos = (max(max_pos[0], pos[0]), max(max_pos[1], pos[1]))

    for rock_path in rock_paths:
        for i in range(len(rock_path)):
            rock_path[i] = (rock_path[i][0] - min_pos[0], rock_path[i][1] - min_pos[1])

    grid: list[list[Type]] = []

    def set_grid(x: int, y: int, ty: Type) -> None:
        while len(grid) <= y:
            grid.append([])

        while len(grid[y]) <= x:
            grid[y].append(Type.EMPTY)

        grid[y][x] = ty

    def get_grid(x: int, y: int) -> Type:
        if y < 0 or y >= len(grid):
            return Type.EMPTY
        elif x < 0 or x >= len(grid[y]):
            return Type.EMPTY
        else:
            return grid[y][x]

    for rock_path in rock_paths:
        for a, b in zip(rock_path, rock_path[1:]):
            if a[0] == b[0]:
                for y in range(min(a[1], b[1]), max(a[1], b[1]) + 1):
                    set_grid(a[0], y, Type.ROCK)
            else:
                for x in range(min(a[0], b[0]), max(a[0], b[0]) + 1):
                    set_grid(x, a[1], Type.ROCK)

    sand_origin = (500 - min_pos[0], -min_pos[1])

    count_sand_at_rest = 0

    while True:
        current_sand_position = sand_origin
        while True:
            if current_sand_position[1] >= max_pos[1] + 1:
                set_grid(current_sand_position[0], current_sand_position[1], Type.SAND)
                count_sand_at_rest += 1
                if current_sand_position == sand_origin:
                    return count_sand_at_rest
                break
            elif get_grid(current_sand_position[0], current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0], current_sand_position[1] + 1)
            elif get_grid(current_sand_position[0] - 1, current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0] - 1, current_sand_position[1] + 1)
            elif get_grid(current_sand_position[0] + 1, current_sand_position[1] + 1) == Type.EMPTY:
                current_sand_position = (current_sand_position[0] + 1, current_sand_position[1] + 1)
            else:
                set_grid(current_sand_position[0], current_sand_position[1], Type.SAND)
                count_sand_at_rest += 1
                if current_sand_position == sand_origin:
                    return count_sand_at_rest
                break


if __name__ == "__main__":
    main()
