use crate::utility;
use std::collections::HashMap;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> (String, HashMap<(char, char), char>) {
	let content = utility::read_lines(format!("{}/day14/{}.txt", base_path_inputs, filename));

	let initial = content[0].clone();
	let mut mapping = HashMap::new();

	for i in 1..content.len() {
		let line = &content[i];
		let chars = line.chars().collect::<Vec<_>>();

		// XY -> Z
		mapping.insert((chars[0], chars[1]), chars[6]);
	}
	(initial, mapping)
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("14.1.example", &part1, input_example.clone());
	utility::benchmark("14.1", &part1, input.clone());

	utility::benchmark("14.2.example", &part2, input_example);
	utility::benchmark("14.2", &part2, input);
}

fn compute_final_lentgh_from_input_lentgh(n: usize, step: usize) -> usize {
	1 + (n - 1) * (1 << step)
}

fn process_polymer(
	mut polymer: &mut Vec<char>,
	mapping: &HashMap<(char, char), char>,
	from: usize,
	to: usize,
) {
	if to - from < 2 {
		return;
	}

	let mid = (to - from) / 2 + from;
	polymer[mid] = mapping[&(polymer[from], polymer[to])];
	process_polymer(&mut polymer, &mapping, from, mid);
	process_polymer(&mut polymer, &mapping, mid, to);
}

fn part1((initial, mapping): (String, HashMap<(char, char), char>)) -> u64 {
	let len = compute_final_lentgh_from_input_lentgh(initial.len(), 10);
	let mut polymer = vec![' '; len];

	let s = (len - 1) / (initial.len() - 1);
	for x in 0..initial.len() {
		polymer[x * s] = initial.chars().nth(x).unwrap();
	}

	for x in 0..initial.len() - 1 {
		process_polymer(&mut polymer, &mapping, x * s, (x + 1) * s);
	}

	let mut count = HashMap::new();
	for c in polymer {
		*count.entry(c).or_insert(0) += 1;
	}

	let mut sorted: Vec<_> = count.values().collect();
	sorted.sort();

	*sorted.last().unwrap() - *sorted[0]
}

fn count_process_polymer(
	mut cache: &mut HashMap<(char, char, usize), HashMap<char, u64>>,
	mut counter: &mut HashMap<char, u64>,
	mapping: &HashMap<(char, char), char>,
	from_value: char,
	to_value: char,
	step_remaining: usize,
) {
	if step_remaining == 0 {
		return;
	}

	if let Some(inner_counter) = cache.get(&(from_value, to_value, step_remaining)) {
		for (k, v) in inner_counter {
			*counter.entry(*k).or_insert(0) += v;
		}
		return;
	}

	let mid_value = mapping[&(from_value, to_value)];

	let mut inner_counter = HashMap::new();

	*inner_counter.entry(mid_value).or_insert(0) += 1;

	count_process_polymer(
		&mut cache,
		&mut inner_counter,
		&mapping,
		from_value,
		mid_value,
		step_remaining - 1,
	);
	count_process_polymer(
		&mut cache,
		&mut inner_counter,
		&mapping,
		mid_value,
		to_value,
		step_remaining - 1,
	);

	for (k, v) in &inner_counter {
		*counter.entry(*k).or_insert(0) += v;
	}

	cache.insert((from_value, to_value, step_remaining), inner_counter);
}

fn part2((initial, mapping): (String, HashMap<(char, char), char>)) -> u64 {
	let chars_initial: Vec<_> = initial.chars().collect();

	let mut counter = HashMap::new();
	let mut cache = HashMap::new();
	for x in 0..initial.len() - 1 {
		count_process_polymer(
			&mut cache,
			&mut counter,
			&mapping,
			chars_initial[x],
			chars_initial[x + 1],
			40,
		);
	}

	for c in initial.chars() {
		*counter.entry(c).or_insert(0) += 1;
	}

	let mut sorted: Vec<_> = counter.values().collect();
	sorted.sort();

	*sorted.last().unwrap() - *sorted[0]
}
