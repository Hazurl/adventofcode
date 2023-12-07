package main

import (
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type HandType int

const (
	HighCard HandType = iota
	OnePair
	TwoPairs
	ThreeOfAKind
	FullHouse
	FourOfAKind
	FiveOfAKind
)

type Card int
type Hand struct {
	cards []Card
	bid   int
}

type JCard int

type JHand struct {
	cards []JCard
	bid   int
}

type ResolvedHand struct {
	hand_type HandType
	cards     []Card
	bid       int
}

type ResolvedJHand struct {
	hand_type HandType
	cards     []JCard
	bid       int
}

func parseCard(input rune) Card {
	if input >= '2' && input <= '9' {
		return Card(input - '0')
	}

	switch input {
	case 'T':
		return 10
	case 'J':
		return 11
	case 'Q':
		return 12
	case 'K':
		return 13
	case 'A':
		return 14

	default:
		panic("Invalid card")
	}
}

func parseHand(input string) Hand {
	cards_bid := strings.Split(input, " ")

	raw_cards := cards_bid[0]
	raw_bid := cards_bid[1]

	cards := make([]Card, len(raw_cards))
	for i, c := range raw_cards {
		cards[i] = parseCard(rune(c))
	}

	bid := panicOnError(strconv.Atoi(raw_bid))

	return Hand{cards, bid}
}

func parseInput(input string) []Hand {
	lines := strings.Split(input, "\n")

	hands := make([]Hand, len(lines))
	for i, line := range lines {
		hands[i] = parseHand(line)
	}

	return hands
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

func (hand *Hand) ResolveHand() ResolvedHand {
	count := make(map[Card]int)

	for _, card := range hand.cards {
		count[card]++
	}

	for _, v := range count {
		if v == 5 {
			return ResolvedHand{FiveOfAKind, hand.cards, hand.bid}
		}
		if v == 4 {
			return ResolvedHand{FourOfAKind, hand.cards, hand.bid}
		}
		if v == 3 {
			for _, v := range count {
				if v == 2 {
					return ResolvedHand{FullHouse, hand.cards, hand.bid}
				}
			}
			return ResolvedHand{ThreeOfAKind, hand.cards, hand.bid}
		}
	}

	for i, v := range count {
		if v == 2 {
			for j, v := range count {
				if v == 2 && i != j {
					return ResolvedHand{TwoPairs, hand.cards, hand.bid}
				}
			}
			return ResolvedHand{OnePair, hand.cards, hand.bid}
		}
	}

	return ResolvedHand{HighCard, hand.cards, hand.bid}
}

func (card Card) AsJCard() JCard {
	if card == 11 {
		return JCard(1)
	}

	return JCard(card)
}

func AsJHand(hand Hand) JHand {
	jcards := make([]JCard, len(hand.cards))
	for i, card := range hand.cards {
		jcards[i] = card.AsJCard()
	}

	return JHand{jcards, hand.bid}
}

func (hand *JHand) ResolveHand() ResolvedJHand {
	count := make(map[JCard]int)
	jokers := 0

	count[1] = 0

	for _, card := range hand.cards {
		if card == 1 {
			jokers++
			continue
		}

		count[card]++
	}

	for _, v := range count {
		if v+jokers == 5 {
			return ResolvedJHand{FiveOfAKind, hand.cards, hand.bid}
		}
	}
	for _, v := range count {
		if v+jokers == 4 {
			return ResolvedJHand{FourOfAKind, hand.cards, hand.bid}
		}
	}
	for i, v := range count {
		if v+jokers == 3 {
			for j, v := range count {
				if v == 2 && i != j {
					return ResolvedJHand{FullHouse, hand.cards, hand.bid}
				}
			}
			return ResolvedJHand{ThreeOfAKind, hand.cards, hand.bid}
		}
	}

	for i, v := range count {
		if v+jokers == 2 {
			for j, v := range count {
				if v == 2 && i != j {
					return ResolvedJHand{TwoPairs, hand.cards, hand.bid}
				}
			}
			return ResolvedJHand{OnePair, hand.cards, hand.bid}
		}
	}

	return ResolvedJHand{HighCard, hand.cards, hand.bid}
}

func part1(input string) int {
	hands := parseInput(input)
	resolved_hands := make([]ResolvedHand, len(hands))

	for i, hand := range hands {
		resolved_hands[i] = hand.ResolveHand()
	}

	sort.Slice(resolved_hands, func(i, j int) bool {
		if resolved_hands[i].hand_type == resolved_hands[j].hand_type {
			for k := 0; k < len(resolved_hands[i].cards); k++ {
				if resolved_hands[i].cards[k] != resolved_hands[j].cards[k] {
					return resolved_hands[i].cards[k] < resolved_hands[j].cards[k]
				}
			}
		}
		return resolved_hands[i].hand_type < resolved_hands[j].hand_type
	})

	sum := 0
	for i, hand := range resolved_hands {
		sum += hand.bid * (i + 1)
	}

	return sum
}

func part2(input string) int {
	hands := parseInput(input)
	jhands := make([]JHand, len(hands))

	for i, hand := range hands {
		jhands[i] = AsJHand(hand)
	}

	resolved_jhands := make([]ResolvedJHand, len(jhands))

	for i, jhand := range jhands {
		resolved_jhands[i] = jhand.ResolveHand()
	}

	sort.Slice(resolved_jhands, func(i, j int) bool {
		if resolved_jhands[i].hand_type == resolved_jhands[j].hand_type {
			for k := 0; k < len(resolved_jhands[i].cards); k++ {
				if resolved_jhands[i].cards[k] != resolved_jhands[j].cards[k] {
					return resolved_jhands[i].cards[k] < resolved_jhands[j].cards[k]
				}
			}
		}
		return resolved_jhands[i].hand_type < resolved_jhands[j].hand_type
	})

	sum := 0
	for i, jhand := range resolved_jhands {
		sum += jhand.bid * (i + 1)
	}

	return sum
}

func main() {
	input := readInput()
	fmt.Println("Part 1: ", part1(input))
	fmt.Println("Part 2: ", part2(input))
}
