package main

import (
	"fmt"
	"math"
)

type Race struct {
	time     int
	distance int
}

func panicOnError[T any](result T, err error) T {
	if err != nil {
		panic(err)
	}
	return result
}

type SolvedRaceResult struct {
	min_time int
	max_time int
}

func SolveRace(race Race) SolvedRaceResult {
	// let t be the time pushing the button
	// let m the maximum time for the race
	// let r the record holder
	// let d_m(t) the distance obtained by pushing the button during t ms for a race of m ms

	// Then:
	// d_m(t) = (m - t) * t = m*t - t^2
	// We need to solve for d_m(t) = r
	// That is t = (m +- sqrt(m^2 - 4*r)) / 2

	m := float64(race.time)
	r := float64(race.distance)

	t1 := (m - math.Sqrt(m*m-4*r)) / 2
	t2 := (m + math.Sqrt(m*m-4*r)) / 2

	// We want the range t1 to t2 were all d_m(t) are **strictly** superior to r
	if math.Ceil(t1) == t1 {
		t1 = t1 + 1
	}

	if math.Floor(t2) == t2 {
		t2 = t2 - 1
	}

	return SolvedRaceResult{
		min_time: int(math.Ceil(t1)),
		max_time: int(math.Floor(t2)),
	}
}

func part1() int {
	races := []Race{
		{42, 308},
		{89, 1170},
		{91, 1291},
		{89, 1467},
	}

	product := 1
	for _, race := range races {
		solved_race := SolveRace(race)
		product = product * (solved_race.max_time - solved_race.min_time + 1)
	}

	return product
}

func part2() int {
	race := Race{
		42899189,
		308117012911467,
	}

	solved_race := SolveRace(race)
	return (solved_race.max_time - solved_race.min_time + 1)
}

func main() {
	fmt.Println("Part 1: ", part1())
	fmt.Println("Part 2: ", part2())
}
