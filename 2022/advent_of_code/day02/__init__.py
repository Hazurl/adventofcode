from enum import Enum


def main() -> None:
    with open("./advent_of_code/day02/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


class RPS(Enum):
    ROCK = 0
    PAPER = 1
    SCISSOR = 2


class Result(Enum):
    DRAW = 0
    WIN = 1
    LOSS = 2


def parse_rps(c: str) -> RPS:
    return {
        "A": RPS.ROCK,
        "B": RPS.PAPER,
        "C": RPS.SCISSOR,
        "X": RPS.ROCK,
        "Y": RPS.PAPER,
        "Z": RPS.SCISSOR,
    }[c]


def parse_result(c: str) -> Result:
    return {
        "X": Result.LOSS,
        "Y": Result.DRAW,
        "Z": Result.WIN,
    }[c]


def compute_result(play: tuple[RPS, RPS]) -> Result:
    result_value = (play[1].value - play[0].value + 3) % 3
    return {
        0: Result.DRAW,
        1: Result.WIN,
        2: Result.LOSS,
    }[result_value]


def compute_play_to_result_in(play: RPS, result: Result) -> RPS:
    result_to_play = {
        Result.DRAW: play,
        Result.WIN: RPS((play.value + 1) % 3),
        Result.LOSS: RPS((play.value - 1) % 3),
    }
    return result_to_play[result]


def compute_score(rps: RPS, result: Result) -> int:
    rps_to_score = {RPS.ROCK: 1, RPS.PAPER: 2, RPS.SCISSOR: 3}
    result_to_score = {Result.DRAW: 3, Result.WIN: 6, Result.LOSS: 0}

    return rps_to_score[rps] + result_to_score[result]


def part1(raw_input: str) -> int:
    def parse_line(line: str) -> tuple[RPS, RPS]:
        lhs, rhs = line.split()
        return parse_rps(lhs), parse_rps(rhs)

    plays = [parse_line(line) for line in raw_input.splitlines() if line.strip()]

    return sum(compute_score(play[1], compute_result(play)) for play in plays)


def part2(raw_input: str) -> int:
    def parse_line(line: str) -> tuple[RPS, Result]:
        lhs, rhs = line.split()
        return parse_rps(lhs), parse_result(rhs)

    plays = [parse_line(line) for line in raw_input.splitlines() if line.strip()]

    return sum(compute_score(compute_play_to_result_in(play[0], play[1]), play[1]) for play in plays)


if __name__ == "__main__":
    main()
