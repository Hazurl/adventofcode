from textwrap import dedent

from advent_of_code import day18


def test_part1():
    assert (
        day18.part1(
            dedent(
                """\
            1,1,1
            2,1,1
            """
            )
        )
        == 10
    )
    assert (
        day18.part1(
            dedent(
                """\
            2,2,2
            1,2,2
            3,2,2
            2,1,2
            2,3,2
            2,2,1
            2,2,3
            2,2,4
            2,2,6
            1,2,5
            3,2,5
            2,1,5
            2,3,5
            """
            )
        )
        == 64
    )


def test_part2():
    assert (
        day18.part2(
            dedent(
                """\
            2,2,2
            1,2,2
            3,2,2
            2,1,2
            2,3,2
            2,2,1
            2,2,3
            2,2,4
            2,2,6
            1,2,5
            3,2,5
            2,1,5
            2,3,5
            """
            )
        )
        == 58
    )
