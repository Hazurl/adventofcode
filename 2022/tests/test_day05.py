from advent_of_code import day05


def test_part1():
    assert (
        day05.part1(
            """
    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
"""
        )
        == "CMZ"
    )


def test_part2():
    assert (
        day05.part2(
            """
    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
    """
        )
        == "MCD"
    )
