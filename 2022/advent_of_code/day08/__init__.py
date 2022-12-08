from dataclasses import dataclass


def main() -> None:
    with open("./advent_of_code/day08/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


def part1(raw_input: str) -> int:
    trees = [[(int(tree), False) for tree in row.strip()] for row in raw_input.strip().splitlines()]
    width = len(trees[0])

    for row in trees:
        max_height = -1
        for index, (tree, _) in enumerate(row):
            if tree > max_height:
                max_height = tree
                row[index] = (tree, True)

        max_height = -1
        for index, (tree, _) in enumerate(row[::-1]):
            if tree > max_height:
                max_height = tree
                row[len(row) - 1 - index] = (tree, True)

    for column in range(width):
        max_height = -1
        for row in trees:
            tree, _ = row[column]
            if tree > max_height:
                max_height = tree
                row[column] = (tree, True)

        max_height = -1
        for row in trees[::-1]:
            tree, _ = row[column]
            if tree > max_height:
                max_height = tree
                row[column] = (tree, True)

    return sum(visible for row in trees for (_, visible) in row)


@dataclass
class TreeVisibles:
    north: int
    south: int
    east: int
    west: int


def part2(raw_input: str) -> int:
    trees = [[int(tree) for tree in row.strip()] for row in raw_input.strip().splitlines()]

    def get_visible_trees(x: int, y: int) -> TreeVisibles:
        height = trees[x][y]
        visible_trees = TreeVisibles(0, 0, 0, 0)
        for other_height in trees[x][0:y][::-1]:
            visible_trees.north += 1
            if other_height >= height:
                break

        for other_height in trees[x][y + 1 :]:
            visible_trees.south += 1
            if other_height >= height:
                break

        for other_height in [row[y] for row in trees[0:x]][::-1]:
            visible_trees.west += 1
            if other_height >= height:
                break

        for other_height in [row[y] for row in trees[x + 1 :]]:
            visible_trees.east += 1
            if other_height >= height:
                break

        return visible_trees

    def get_score(visible_trees: TreeVisibles) -> int:
        return visible_trees.north * visible_trees.south * visible_trees.east * visible_trees.west

    return max(get_score(get_visible_trees(x, y)) for x in range(len(trees)) for y in range(len(trees[x])))


if __name__ == "__main__":
    main()
