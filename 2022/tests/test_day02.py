from advent_of_code import day02


def test_part1():
    assert (
        day02.part1(
            """
    A Y
    B X
    C Z
    """
        )
        == 15
    )


def test_part2():
    assert (
        day02.part2(
            """
    A Y
    B X
    C Z
    """
        )
        == 12
    )
