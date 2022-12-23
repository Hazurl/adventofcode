from textwrap import dedent

from advent_of_code import day20


def test_part1():
    assert (
        day20.part1(
            dedent(
                """\
            1
            2
            -3
            3
            -2
            0
            4
            """
            )
        )
        == 3
    )


def test_part2():
    assert (
        day20.part2(
            dedent(
                """\
            1
            2
            -3
            3
            -2
            0
            4
            """
            )
        )
        == 1623178306
    )
