from textwrap import dedent

from advent_of_code import day09


def test_part1():
    assert (
        day09.part1(
            dedent(
                """\
                R 4
                U 4
                L 3
                D 1
                R 4
                D 1
                L 5
                R 2
            """
            )
        )
        == 13
    )


def test_part2():
    assert (
        day09.part2(
            dedent(
                """\
                R 5
                U 8
                L 8
                D 3
                R 17
                D 10
                L 25
                U 20
            """
            )
        )
        == 36
    )
