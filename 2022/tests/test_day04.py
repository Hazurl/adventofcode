from advent_of_code import day04


def test_part1():
    assert (
        day04.part1(
            """
2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
    """
        )
        == 2
    )


def test_part2():
    assert (
        day04.part2(
            """
2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
    """
        )
        == 4
    )
