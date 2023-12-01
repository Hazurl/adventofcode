package main

import (
	"fmt"
	"os"
	"strings"
)

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

func GetDigitsOnly(input string) string {
	digits := ""
	for _, char := range input {
		if '0' <= char && char <= '9' {
			digits += string(char)
		}
	}
	return digits
}

func part1(input string) int {
	sum_calibration_value := 0

	for line_num, line := range strings.Split(input, "\n") {
		digits := GetDigitsOnly(line)
		if len(digits) < 1 {
			panic(fmt.Sprintf("Not enough digits in line %d (%s)", line_num, line))
		}

		first_digit := int(digits[0]) - '0'
		last_digit := int(digits[len(digits)-1]) - '0'

		calibration_value := first_digit*10 + last_digit
		sum_calibration_value += calibration_value
	}

	return sum_calibration_value
}

func GetWrittenDigit(input string) (int, error) {
	if len(input) >= 1 && '0' <= input[0] && input[0] <= '9' {
		return int(input[0]) - '0', nil
	}

	written_digits := []string{"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"}

	for i, digit := range written_digits {
		if strings.HasPrefix(input, digit) {
			return i, nil
		}
	}

	return 0, fmt.Errorf("No digit found in line")
}

func GetAllDigits(input string) []int {
	digits := []int{}

	for index := 0; index < len(input); index++ {
		word := input[index:]
		digit, err := GetWrittenDigit(word)
		if err != nil {
			continue
		}
		digits = append(digits, digit)
	}

	return digits
}

func part2(input string) int {
	sum_calibration_value := 0

	for line_num, line := range strings.Split(input, "\n") {
		digits := GetAllDigits(line)

		if len(digits) < 1 {
			panic(fmt.Sprintf("Not enough digits in line %d (%s)", line_num, line))
		}

		first_digit := digits[0]
		last_digit := digits[len(digits)-1]

		calibration_value := first_digit*10 + last_digit
		sum_calibration_value += calibration_value
	}

	return sum_calibration_value
}

func main() {
	input := readInput()
	fmt.Println("Part 1: ", part1(input))
	fmt.Println("Part 2: ", part2(input))
}
