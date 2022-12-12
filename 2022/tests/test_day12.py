from textwrap import dedent

from advent_of_code import day12


def test_part1():
    assert (
        day12.part1(
            dedent(
                """\
            Sabqponm
            abcryxxl
            accszExk
            acctuvwj
            abdefghi
            """
            )
        )
        == 31
    )


def test_part2():
    assert (
        day12.part2(
            dedent(
                """\
            Sabqponm
            abcryxxl
            accszExk
            acctuvwj
            abdefghi
            """
            )
        )
        == 29
    )
