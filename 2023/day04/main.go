package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Card struct {
	id               int
	winning_numbers  []int
	obtained_numbers []int
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

func parseCard(input string) Card {
	input = strings.TrimPrefix(input, "Card ")
	input = strings.TrimLeft(input, " ")
	id_other_str := strings.Split(input, ": ")
	id := panicOnError(strconv.Atoi(id_other_str[0]))
	input = id_other_str[1]

	winning_numbers_obtained_numbers := strings.Split(input, " | ")
	winning_numbers_str := strings.Split(winning_numbers_obtained_numbers[0], " ")
	obtained_numbers_str := strings.Split(winning_numbers_obtained_numbers[1], " ")

	winning_numbers := []int{}
	for _, number_str := range winning_numbers_str {
		if number_str == "" {
			continue
		}
		winning_numbers = append(winning_numbers, panicOnError(strconv.Atoi(number_str)))
	}

	obtained_numbers := []int{}
	for _, number_str := range obtained_numbers_str {
		if number_str == "" {
			continue
		}
		obtained_numbers = append(obtained_numbers, panicOnError(strconv.Atoi(number_str)))
	}

	return Card{
		id:               id,
		winning_numbers:  winning_numbers,
		obtained_numbers: obtained_numbers,
	}
}

func parseInput(input string) []Card {
	cards := []Card{}

	for _, card := range strings.Split(input, "\n") {
		cards = append(cards, parseCard(card))
	}

	return cards
}

func GetNumbersOfMatches(card Card) int {
	winning_numbers := map[int]bool{}
	for _, number := range card.winning_numbers {
		winning_numbers[number] = true
	}

	matches := 0
	for _, number := range card.obtained_numbers {
		if winning_numbers[number] {
			matches += 1
		}
	}

	return matches
}

func GetScore(card Card) int {
	matches := GetNumbersOfMatches(card)
	if matches == 0 {
		return 0
	} else {
		return 1 << (matches - 1)
	}
}

func part1(input string) int {
	cards := parseInput(input)

	score := 0
	for _, card := range cards {
		score += GetScore(card)
	}

	return score
}

func part2(input string) int {
	cards := parseInput(input)

	cards_count := map[int]int{}

	for _, card := range cards {
		cards_count[card.id] = 1
	}

	for _, card := range cards {
		matches := GetNumbersOfMatches(card)
		for i := 1; i <= matches; i++ {
			cards_count[card.id+i] += cards_count[card.id]
		}
	}

	total_cards_count := 0

	for _, count := range cards_count {
		total_cards_count += count
	}

	return total_cards_count
}

func main() {
	input := readInput()
	fmt.Println("Part 1: ", part1(input))
	fmt.Println("Part 2: ", part2(input))
}
