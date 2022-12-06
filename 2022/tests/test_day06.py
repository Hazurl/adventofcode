from advent_of_code import day06


def test_part1():
    assert day06.part1("bvwbjplbgvbhsrlpgdmjqwftvncz\n") == 5
    assert day06.part1("nppdvjthqldpwncqszvftbrmjlhg\n") == 6
    assert day06.part1("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg\n") == 10
    assert day06.part1("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw\n") == 11


def test_part2():
    assert day06.part2("mjqjpqmgbljsphdztnvjfqwrcgsmlb\n") == 19
    assert day06.part2("bvwbjplbgvbhsrlpgdmjqwftvncz\n") == 23
    assert day06.part2("nppdvjthqldpwncqszvftbrmjlhg\n") == 23
    assert day06.part2("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg\n") == 29
    assert day06.part2("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw\n") == 26
