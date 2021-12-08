use crate::utility;
use regex::Regex;
use std::collections::HashMap;
use std::collections::HashSet;
use std::vec::Vec;

#[derive(Clone, Debug)]
struct SignalPattern {
	segments: Vec<char>,
}

#[derive(Clone, Debug)]
struct Entry {
	all_digits: Vec<SignalPattern>,
	output: Vec<SignalPattern>,
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<Entry> {
	let regex = Regex::new("[abcdefg]+").unwrap();
	let content = utility::read_lines(format!("{}/day8/{}.txt", base_path_inputs, filename));

	let mut entries = Vec::<Entry>::new();

	for line in content {
		let mut entry = Entry {
			all_digits: Vec::new(),
			output: Vec::new(),
		};

		for cap in regex.captures_iter(&line) {
			let signal_pattern = SignalPattern {
				segments: String::from(&cap[0]).chars().collect(),
			};

			if entry.all_digits.len() < 10 {
				entry.all_digits.push(signal_pattern);
			} else {
				entry.output.push(signal_pattern);
			}
		}

		entries.push(entry);
	}

	entries
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("8.1.example", &part1, input_example.clone());
	utility::benchmark("8.1", &part1, input.clone());

	utility::benchmark("8.2.example", &part2, input_example);
	utility::benchmark("8.2", &part2, input);
}

fn part1(entries: Vec<Entry>) -> u64 {
	let mut count = 0;

	for entry in entries {
		for output in entry.output {
			let len = output.segments.len();
			if len == 2 || len == 3 || len == 4 || len == 7 {
				count += 1;
			}
		}
	}
	count
}

#[derive(Copy, Clone, Hash, Eq, PartialEq, Debug)]
enum Segment {
	T,
	TR,
	BR,
	B,
	BL,
	TL,
	C,
}

fn mk_set(vec: Vec<Segment>) -> HashSet<Segment> {
	HashSet::from_iter(vec.iter().copied())
}

fn segments_to_digit(segments: &HashSet<Segment>) -> Option<u64> {
	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TR,
			Segment::BR,
			Segment::B,
			Segment::BL,
			Segment::TL,
		]) {
		return Some(0);
	}

	if segments == &mk_set(vec![Segment::TR, Segment::BR]) {
		return Some(1);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TR,
			Segment::C,
			Segment::B,
			Segment::BL,
		]) {
		return Some(2);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TR,
			Segment::C,
			Segment::BR,
			Segment::B,
		]) {
		return Some(3);
	}

	if segments == &mk_set(vec![Segment::TR, Segment::TL, Segment::C, Segment::BR]) {
		return Some(4);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TL,
			Segment::C,
			Segment::BR,
			Segment::B,
		]) {
		return Some(5);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::BL,
			Segment::BR,
			Segment::C,
			Segment::B,
			Segment::TL,
		]) {
		return Some(6);
	}

	if segments == &mk_set(vec![Segment::T, Segment::TR, Segment::BR]) {
		return Some(7);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TR,
			Segment::BR,
			Segment::C,
			Segment::B,
			Segment::BL,
			Segment::TL,
		]) {
		return Some(8);
	}

	if segments
		== &mk_set(vec![
			Segment::T,
			Segment::TR,
			Segment::BR,
			Segment::B,
			Segment::C,
			Segment::TL,
		]) {
		return Some(9);
	}

	return None;
}

fn signal_to_set(signal: &SignalPattern) -> HashSet<char> {
	HashSet::<char>::from_iter(signal.segments.iter().cloned())
}

fn find_common(lhs: &HashSet<char>, rhs: &HashSet<char>) -> HashSet<char> {
	lhs.intersection(&rhs).cloned().collect()
}

fn without(lhs: &HashSet<char>, rhs: &HashSet<char>) -> HashSet<char> {
	let mut res = lhs.clone();
	for e in rhs {
		res.remove(e);
	}

	res
}

fn remove_already_found(
	mut set: HashSet<char>,
	already_found: &HashMap<char, Segment>,
) -> HashSet<char> {
	for (found, _) in already_found {
		set.remove(found);
	}

	set
}

fn ensure_only_solution(set: HashSet<char>) -> char {
	if set.len() != 1 {
		unreachable!();
	}

	set.into_iter().fold(' ', |_, n| n)
}

fn solve(mut entry: Entry) -> u64 {
	entry
		.all_digits
		.sort_by(|lhs, rhs| lhs.segments.len().cmp(&rhs.segments.len()));

	let signal_1 = signal_to_set(&entry.all_digits[0]);
	let signal_7 = signal_to_set(&entry.all_digits[1]);
	let signal_4 = signal_to_set(&entry.all_digits[2]);
	let signal_2 = signal_to_set(&entry.all_digits[3]); // Don't care about the order of 0, 6, and 9
	let signal_3 = signal_to_set(&entry.all_digits[4]);
	let signal_5 = signal_to_set(&entry.all_digits[5]);
	let signal_0 = signal_to_set(&entry.all_digits[6]); // Don't care about the order of 0, 6, and 9
	let signal_6 = signal_to_set(&entry.all_digits[7]);
	let signal_9 = signal_to_set(&entry.all_digits[8]);
	let signal_8 = signal_to_set(&entry.all_digits[9]);

	let common_0_6_9 = find_common(&signal_0, &find_common(&signal_6, &signal_9));
	let common_2_3_5 = find_common(&signal_2, &find_common(&signal_5, &signal_3));

	let mut letter_to_segment = HashMap::<char, Segment>::new();
	letter_to_segment.insert(
		ensure_only_solution(without(&signal_7, &signal_1)),
		Segment::T,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(
			find_common(&common_0_6_9, &signal_1),
			&letter_to_segment,
		)),
		Segment::BR,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(
			find_common(&common_0_6_9, &signal_4),
			&letter_to_segment,
		)),
		Segment::TL,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(signal_1.clone(), &letter_to_segment)),
		Segment::TR,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(signal_4.clone(), &letter_to_segment)),
		Segment::C,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(
			common_2_3_5.clone(),
			&letter_to_segment,
		)),
		Segment::B,
	);

	letter_to_segment.insert(
		ensure_only_solution(remove_already_found(signal_8.clone(), &letter_to_segment)),
		Segment::BL,
	);

	let mut total = 0;

	for output_signal in &entry.output {
		total *= 10;

		let set = signal_to_set(output_signal);
		let segments = set
			.into_iter()
			.map(|l| letter_to_segment.get(&l).unwrap())
			.copied()
			.collect();

		let digit = segments_to_digit(&segments);
		total += digit.unwrap();
	}

	total
}

fn part2(entries: Vec<Entry>) -> u64 {
	entries.into_iter().map(|e| solve(e)).sum()
}
