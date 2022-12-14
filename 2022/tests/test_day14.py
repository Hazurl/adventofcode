from textwrap import dedent

from advent_of_code import day14


def test_part1():
    assert (
        day14.part1(
            dedent(
                """\
            498,4 -> 498,6 -> 496,6
            503,4 -> 502,4 -> 502,9 -> 494,9
            """
            )
        )
        == 24
    )


def test_part2():
    assert (
        day14.part2(
            dedent(
                """\
            498,4 -> 498,6 -> 496,6
            503,4 -> 502,4 -> 502,9 -> 494,9
            """
            )
        )
        == 93
    )
