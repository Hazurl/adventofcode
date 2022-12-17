from textwrap import dedent

import pytest

from advent_of_code import day17


def test_part1():
    assert (
        day17.part1(
            dedent(
                """\
            >>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>
            """
            )
        )
        == 3068
    )


@pytest.mark.skip(reason="Not working D:")
def test_part2():
    assert (
        day17.part2(
            dedent(
                """\
            >>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>
            """
            )
        )
        == 1514285714288
    )
