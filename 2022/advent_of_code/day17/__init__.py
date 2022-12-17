from typing import Iterable, Optional, TypeVar


def main() -> None:
    with open("./advent_of_code/day17/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


class Grid:
    def __init__(self):
        self.rows = []

    def _add_row(self):
        self.rows.append([False] * 7)

    def get(self, x: int, y: int) -> bool:
        if x < 0 or x >= 7:
            return True
        elif y < 0:
            return True
        elif y >= len(self.rows):
            return False
        else:
            return self.rows[y][x]

    def set(self, x: int, y: int):
        while len(self.rows) <= y:
            self._add_row()

        assert 0 <= x < 7

        assert not self.rows[y][x]
        self.rows[y][x] = True

    def get_origin(self) -> tuple[int, int]:
        return (2, len(self.rows) + 3)

    def print_it(self, blocks: Optional[Iterable[tuple[int, int]]] = None):
        blocks_positions = set(blocks) if blocks is not None else set()

        for y in range(len(self.rows) + 3, -1, -1):
            row = self.rows[y] if y < len(self.rows) else [False] * 7
            print("#", end="")
            for x, cell in enumerate(row):
                c = "#" if cell else "."
                if (x, y) in blocks_positions:
                    c = "@"
                print(c, end="")

            print("#")
        print("#" * 9)

    def compute_hash(self) -> tuple[int, ...]:
        # For each column, find the index of the block at the top
        # The index is relative to the top-most row
        return tuple(
            next((index_relative_to_top for index_relative_to_top, row in enumerate(self.rows[::-1]) if row[col]), -1) for col in range(7)
        )


class Shape:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def down(self):
        self.y -= 1

    def left(self):
        self.x -= 1

    def right(self):
        self.x += 1

    def up(self):
        self.y += 1

    def get_relative_blocks(self) -> list[tuple[int, int]]:
        raise NotImplementedError

    def blocks(self) -> Iterable[tuple[int, int]]:
        return [(dx + self.x, dy + self.y) for dx, dy in self.get_relative_blocks()]

    def is_colliding(self, grid: Grid) -> bool:
        for x, y in self.blocks():
            if grid.get(x, y):
                return True

        return False

    def place(self, grid: Grid):
        for x, y in self.blocks():
            grid.set(x, y)


class HorizontalShape(Shape):
    def get_relative_blocks(self) -> list[tuple[int, int]]:
        return [
            (0, 0),
            (1, 0),
            (2, 0),
            (3, 0),
        ]


class PlusShape(Shape):
    def get_relative_blocks(self) -> list[tuple[int, int]]:
        return [
            (1, 1),
            (1, 0),
            (1, 2),
            (0, 1),
            (2, 1),
        ]


class JShape(Shape):
    def get_relative_blocks(self) -> list[tuple[int, int]]:
        return [
            (0, 0),
            (1, 0),
            (2, 0),
            (2, 1),
            (2, 2),
        ]


class VerticalShape(Shape):
    def get_relative_blocks(self) -> list[tuple[int, int]]:
        return [
            (0, 0),
            (0, 1),
            (0, 2),
            (0, 3),
        ]


class CubeShape(Shape):
    def get_relative_blocks(self) -> list[tuple[int, int]]:
        return [
            (0, 0),
            (1, 0),
            (0, 1),
            (1, 1),
        ]


shapes_order = [
    HorizontalShape,
    PlusShape,
    JShape,
    VerticalShape,
    CubeShape,
]

T = TypeVar("T")


def repeated(iterable: Iterable[T]) -> Iterable[T]:
    while True:
        for i in iterable:
            yield i


def part1(raw_input: str) -> int:
    jets = raw_input.strip()

    def play_shape(shape: Shape, jets: Iterable[str]):
        for j in jets:
            if j == ">":
                shape.right()
            else:
                assert j == "<"
                shape.left()

            if shape.is_colliding(grid):
                # Revert
                if j == "<":
                    shape.right()
                else:
                    shape.left()

            shape.down()
            if shape.is_colliding(grid):
                # Revert
                shape.up()
                shape.place(grid)
                return

    grid = Grid()
    infinite_jets = repeated(jets)
    for i, shape_constructor in enumerate(repeated(shapes_order)):
        if i == 2022:
            break
        shape = shape_constructor(*grid.get_origin())
        play_shape(shape, infinite_jets)

    return len(grid.rows)


def part2(raw_input: str) -> int:
    # Doesn't work...
    jets = raw_input.strip()

    def play_shape(shape: Shape, index_jets: int):
        while True:
            j = jets[index_jets % len(jets)]
            index_jets += 1

            if j == ">":
                shape.right()
            else:
                assert j == "<"
                shape.left()

            if shape.is_colliding(grid):
                # Revert
                if j == "<":
                    shape.right()
                else:
                    shape.left()

            shape.down()
            if shape.is_colliding(grid):
                # Revert
                shape.up()
                shape.place(grid)
                return

    grid = Grid()
    input_hashes: dict[tuple[int, ...], tuple[int, int]] = dict()

    max_i = 1000000000000

    for i, shape_constructor in enumerate(repeated(shapes_order)):
        index_jets = i % len(jets)
        index_shape = i % len(shapes_order)

        if index_jets == 0 and index_shape == 0:
            grid_hash = grid.compute_hash()

            print(f"Hash: {grid_hash}")
            if -1 not in grid_hash and grid_hash in input_hashes:
                hi, hl = input_hashes[grid_hash]
                gl = len(grid.rows)
                dl = len(grid.rows) - hl
                di = i - hi
                mi = max_i - i
                print(f"Found loop: {i}={gl}, {hi}={hl}, ~{di}={dl}, {mi}")
                print(f"Expected next loop: {i+di}={gl+dl}")

                steps = mi // di
                print(f"Steps: {steps}")
                gl += steps * dl
                i += steps * di
                print(f"Got {i}={gl}")
                # TODO: Do the remaining steps
                # In the test case i == max_i, but I get the wrong result, this
                # means, the grid_hash is not unique enough
                # and the solution is not correct
                return gl
            else:
                input_hashes[grid_hash] = (i, len(grid.rows))

        shape = shape_constructor(*grid.get_origin())
        play_shape(shape, index_jets)

    return len(grid.rows)


if __name__ == "__main__":
    main()
