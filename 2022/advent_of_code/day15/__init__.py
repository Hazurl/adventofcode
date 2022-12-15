import re
from dataclasses import dataclass


def main() -> None:
    with open("./advent_of_code/day15/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input, 2000000)}")
    print(f"Part 2: {part2(raw_input)}")


regex = re.compile(r"Sensor at x=(?P<x>-?\d+), y=(?P<y>-?\d+): closest beacon is at x=(?P<bx>-?\d+), y=(?P<by>-?\d+)")

Pos = tuple[int, int]


def distance(a: Pos, b: Pos) -> int:
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


@dataclass
class SensorRead:
    sensor: Pos
    beacon: Pos

    def distance(self) -> int:
        return distance(self.sensor, self.beacon)


def parse_line(line: str) -> SensorRead:
    match = regex.match(line)
    if match is None:
        raise ValueError(f"Invalid line: {line}")

    return SensorRead(
        sensor=(int(match["x"]), int(match["y"])),
        beacon=(int(match["bx"]), int(match["by"])),
    )


def part1(raw_input: str, which_row: int) -> int:
    sensor_reads = [parse_line(line) for line in raw_input.strip().splitlines()]

    min_x, max_x = min(min(sr.sensor[0], sr.beacon[0]) for sr in sensor_reads), max(max(sr.sensor[0], sr.beacon[0]) for sr in sensor_reads)

    sensor_positions = set(sr.sensor for sr in sensor_reads)
    beacon_positions = set(sr.beacon for sr in sensor_reads)
    max_distance = max(sr.distance() for sr in sensor_reads)

    sensors_projected_to_row = [(sr.sensor[0], sr.distance() - distance(sr.sensor, (sr.sensor[0], which_row))) for sr in sensor_reads]

    ranges_covered_by_sensors_in_row = [(x - dist, x + dist) for (x, dist) in sensors_projected_to_row if dist >= 0]

    in_range_of_any_sensor_count = 0
    for i in range(min_x - max_distance, max_x + max_distance + 1):

        if (i, which_row) in beacon_positions:
            continue

        if (i, which_row) in sensor_positions:
            continue

        in_range_of_any_sensor = any(start <= i <= end for (start, end) in ranges_covered_by_sensors_in_row)
        if in_range_of_any_sensor:
            in_range_of_any_sensor_count += 1

    return in_range_of_any_sensor_count


def part2(raw_input: str) -> int:
    sensor_reads = [parse_line(line) for line in raw_input.strip().splitlines()]

    row_index = 0
    while True:
        if row_index >= 4000000:
            raise ValueError("No solution found")

        sensors_projected_to_row = [(sr.sensor[0], sr.distance() - distance(sr.sensor, (sr.sensor[0], row_index))) for sr in sensor_reads]

        ranges_covered_by_sensors_in_row = sorted(
            [(x - dist, x + dist) for (x, dist) in sensors_projected_to_row if dist >= 0], key=lambda x: x[0]
        )

        x = 0
        dx = None
        for r in ranges_covered_by_sensors_in_row:
            if x < r[0] - 1:
                return row_index + 4000000 * (x + 1)

            if x < r[1]:
                if dx is None:
                    dx = r[1] - x
                else:
                    dx = min(dx, r[1] - x)
                x = r[1]

        row_index += dx // 2 if dx is not None and dx > 1 else 1


if __name__ == "__main__":
    main()

# 12625383204261
