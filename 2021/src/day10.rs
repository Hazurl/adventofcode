use crate::utility;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<String> {
	utility::read_lines(format!("{}/day10/{}.txt", base_path_inputs, filename))
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("10.1.example", &part1, input_example.clone());
	utility::benchmark("10.1", &part1, input.clone());

	utility::benchmark("10.2.example", &part2, input_example);
	utility::benchmark("10.2", &part2, input);
}

fn is_open_bracket(c: char) -> bool {
	c == '(' || c == '[' || c == '{' || c == '<'
}

fn is_close_bracket(c: char) -> bool {
	c == ')' || c == ']' || c == '}' || c == '>'
}

fn open_to_close_bracket(c: char) -> char {
	match c {
		'(' => ')',
		'[' => ']',
		'{' => '}',
		'<' => '>',
		_ => unreachable!(),
	}
}

fn find_illegal_character(str: &String) -> Option<char> {
	let mut stack = Vec::new();

	for c in str.chars() {
		if is_open_bracket(c) {
			stack.push(open_to_close_bracket(c));
		} else if is_close_bracket(c) {
			if *stack.last().unwrap() == c {
				stack.pop();
			} else {
				return Some(c);
			}
		} else {
			unreachable!();
		}
	}

	None
}

fn part1(lines: Vec<String>) -> u64 {
	let mut score = 0;

	for line in lines {
		score += match find_illegal_character(&line) {
			Some(')') => 3,
			Some(']') => 57,
			Some('}') => 1197,
			Some('>') => 25137,
			None => 0,
			_ => unreachable!(),
		}
	}
	score
}

fn calculate_autocomplete_score(incompleted: &Vec<char>) -> u64 {
	let mut score = 0;

	for c in incompleted.into_iter().rev() {
		score = score * 5
			+ match c {
				')' => 1,
				']' => 2,
				'}' => 3,
				'>' => 4,
				_ => unreachable!(),
			}
	}

	score
}

fn get_autocomplete_score(str: &String) -> Option<u64> {
	let mut stack = Vec::new();

	for c in str.chars() {
		if is_open_bracket(c) {
			stack.push(open_to_close_bracket(c));
		} else if is_close_bracket(c) {
			if *stack.last().unwrap() == c {
				stack.pop();
			} else {
				return None;
			}
		} else {
			unreachable!();
		}
	}

	Some(calculate_autocomplete_score(&stack))
}

fn part2(lines: Vec<String>) -> u64 {
	let mut scores = lines
		.into_iter()
		.filter_map(|line| get_autocomplete_score(&line))
		.collect::<Vec<u64>>();
	scores.sort();

	scores[(scores.len() - 1) / 2]
}
