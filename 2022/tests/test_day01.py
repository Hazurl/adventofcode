from advent_of_code import day01


def test_part1():
    assert day01.part1("123\n456\n789\n\n1000\n1000\n1000") == 3000
    assert (
        day01.part1(
            """
    1000
    2000
    3000

    4000

    5000
    6000

    7000
    8000
    9000

    10000
    """
        )
        == 24000
    )


def test_part2():
    assert (
        day01.part2(
            """
    1000
    2000
    3000

    4000

    5000
    6000

    7000
    8000
    9000

    10000
    """
        )
        == 45000
    )
