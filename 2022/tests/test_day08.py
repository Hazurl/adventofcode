from textwrap import dedent

from advent_of_code import day08


def test_part1():
    assert (
        day08.part1(
            dedent(
                """\
                30373
                25512
                65332
                33549
                35390
            """
            )
        )
        == 21
    )


def test_part2():
    assert (
        day08.part2(
            dedent(
                """\
                30373
                25512
                65332
                33549
                35390
            """
            )
        )
        == 8
    )
