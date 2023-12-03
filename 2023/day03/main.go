package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

type CellType int

const (
	Empty CellType = iota
	Digit
	Symbol
)

type Cell struct {
	value rune
	x     int
	y     int
	id    int
}
type Engine [][]rune

func (c Cell) Type() CellType {
	if c.value == '.' {
		return Empty
	} else if '0' <= c.value && c.value <= '9' {
		return Digit
	} else {
		return Symbol
	}
}

func (e Engine) Get(x, y int) Cell {
	value := '.'
	if y >= 0 && y < len(e) && x >= 0 && x < len(e[y]) {
		value = e[y][x]
	}

	return Cell{
		value: value,
		x:     x,
		y:     y,
		id:    y*len(e[0]) + x,
	}
}

type Number struct {
	value int
	x     int
	y     int
	len   int
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

func parseInput(input string) Engine {
	engine := Engine{}

	for _, line := range strings.Split(input, "\n") {
		row := []rune{}
		for _, char := range line {
			row = append(row, char)
		}
		engine = append(engine, row)
	}

	return engine
}

func (e Engine) CellsOfRow(row int) <-chan Cell {
	ch := make(chan Cell)

	go func() {
		defer close(ch)
		for x := range e[row] {
			ch <- e.Get(x, row)
		}
	}()

	return ch
}

func (e Engine) CollectNumbers() []Number {
	numbers := []Number{}

	TryAppendNumber := func(str string, x, y int) {
		if len(str) > 0 {
			numbers = append(numbers, Number{
				value: panicOnError(strconv.Atoi(str)),
				x:     x - len(str),
				y:     y,
				len:   len(str),
			})
		}
	}

	for row, cells := range e {
		current_number := ""
		for cell := range e.CellsOfRow(row) {
			if cell.Type() == Digit {
				current_number += string(cell.value)
			} else if len(current_number) > 0 {
				TryAppendNumber(current_number, cell.x, cell.y)
				current_number = ""
			}
		}

		TryAppendNumber(current_number, len(cells), row)
	}

	return numbers
}

func (e Engine) AdjacentCells(number Number) <-chan Cell {
	ch := make(chan Cell)

	go func() {
		defer close(ch)
		for dy := -1; dy <= 1; dy++ {
			for dx := -1; dx <= number.len; dx++ {
				if dy == 0 && dx >= 0 && dx < number.len {
					continue
				}
				ch <- e.Get(number.x+dx, number.y+dy)
			}
		}
	}()

	return ch
}

func (e Engine) IsPartNumber(number Number) bool {
	for cell := range e.AdjacentCells(number) {
		if cell.Type() == Symbol {
			return true
		}
	}
	return false
}

func part1(input string) int {
	engine := parseInput(input)
	numbers := engine.CollectNumbers()

	sum := 0

	for _, number := range numbers {
		if engine.IsPartNumber(number) {
			sum += number.value
		}
	}

	return sum
}

func part2(input string) int {
	engine := parseInput(input)
	numbers := engine.CollectNumbers()

	gearsNumbers := map[int][]Number{}

	for _, number := range numbers {
		for cell := range engine.AdjacentCells(number) {
			if cell.Type() == Symbol && cell.value == '*' {
				gearsNumbers[cell.id] = append(gearsNumbers[cell.id], number)
			}
		}
	}

	sum := 0

	for _, gear := range gearsNumbers {
		if len(gear) == 2 {
			sum += gear[0].value * gear[1].value
		}
	}

	return sum
}

func main() {
	input := readInput()
	fmt.Println("Part 1: ", part1(input))
	fmt.Println("Part 2: ", part2(input))
}
