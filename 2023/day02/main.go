package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Game struct {
	id     int
	rounds []Round
}

type Round struct {
	red   int
	blue  int
	green int
}

func (r Round) String() string {
	return fmt.Sprintf("%d red, %d blue, %d green", r.red, r.blue, r.green)
}

func (g Game) String() string {
	rounds := []string{}
	for _, round := range g.rounds {
		rounds = append(rounds, round.String())
	}
	return fmt.Sprintf("Game %d: %s", g.id, strings.Join(rounds, "; "))
}

func panicOnError[T any](result T, err error) T {
	if err != nil {
		panic(err)
	}
	return result
}

func readInput() string {
	dat := panicOnError(os.ReadFile("input.txt"))
	return strings.TrimRight(string(dat), "\n")
}

func parseInput(input string) []Game {
	games := []Game{}

	for _, line := range strings.Split(input, "\n") {
		games = append(games, parseGame(line))
	}

	return games
}

func parseGame(line string) Game {
	// `Game <id>: <rounds>`
	// `rounds`: `<round>; <round>; ...`
	// `round`: `<count> <color>, <count> <color>, ...`
	// `color`: `red`, `blue`, `green`

	line = strings.TrimPrefix(line, "Game ")
	id_rounds := strings.Split(line, ": ")
	id := panicOnError(strconv.Atoi(id_rounds[0]))
	rounds := strings.Split(id_rounds[1], "; ")

	rounds_parsed := []Round{}
	for _, round := range rounds {
		cubes := strings.Split(round, ", ")
		red := 0
		blue := 0
		green := 0
		for _, cube := range cubes {
			count_color := strings.Split(cube, " ")
			count := panicOnError(strconv.Atoi(count_color[0]))
			switch count_color[1] {
			case "red":
				red = count
			case "blue":
				blue = count
			case "green":
				green = count
			default:
				panic(fmt.Sprintf("Unknown color: %s", count_color[1]))
			}
		}
		rounds_parsed = append(rounds_parsed, Round{red, blue, green})
	}

	return Game{id, rounds_parsed}
}

func part1(input string) int {
	games := parseInput(input)

	sum_ids := 0

	for _, game := range games {
		is_possible := true

		for _, round := range game.rounds {
			if round.blue > 14 || round.green > 13 || round.red > 12 {
				is_possible = false
				break
			}
		}

		if is_possible {
			sum_ids += game.id
		}
	}

	return sum_ids
}

func part2(input string) int {
	games := parseInput(input)

	sum_power := 0

	for _, game := range games {
		max_red := game.rounds[0].red
		max_green := game.rounds[0].green
		max_blue := game.rounds[0].blue

		for _, round := range game.rounds {
			if round.red > max_red {
				max_red = round.red
			}
			if round.green > max_green {
				max_green = round.green
			}
			if round.blue > max_blue {
				max_blue = round.blue
			}
		}

		power := max_red * max_green * max_blue
		sum_power += power
	}

	return sum_power
}

func main() {
	input := readInput()
	fmt.Println("Part 1: ", part1(input))
	fmt.Println("Part 2: ", part2(input))
}
