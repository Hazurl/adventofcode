use crate::utility;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<u64> {
	utility::read_lines(format!("{}/day7/{}.txt", base_path_inputs, filename))[0]
		.split(',')
		.map(|n| n.parse().unwrap())
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("7.1.example", &part1, input_example.clone());
	utility::benchmark("7.1", &part1, input.clone());

	utility::benchmark("7.2.example", &part2, input_example);
	utility::benchmark("7.2", &part2, input);
}

fn calculate_fuel_part_1(crabs: &Vec<u64>, pos: u64) -> u64 {
	let mut sum = 0;

	for crab in crabs {
		sum += if *crab < pos { pos - *crab } else { crab - pos };
	}

	sum
}

fn find_best_position_part_1(crabs: &Vec<u64>) -> u64 {
	if crabs.len() % 2 == 0 {
		crabs[crabs.len() / 2 - 1]
	} else {
		let idx = (crabs.len() - 1) / 2;
		((crabs[idx] + crabs[idx + 1]) as f32 / 2.0).round() as u64
	}
}

fn part1(mut crabs: Vec<u64>) -> u64 {
	crabs.sort();
	calculate_fuel_part_1(&crabs, find_best_position_part_1(&crabs))
}

fn calculate_fuel_part_2(crabs: &Vec<u64>, pos: u64) -> u64 {
	let mut sum = 0;

	for crab in crabs {
		let diff = if *crab < pos { pos - *crab } else { crab - pos };
		sum += (diff * (diff + 1)) / 2;
	}

	sum
}

fn part2(mut crabs: Vec<u64>) -> u64 {
	crabs.sort();

	let mut min_fuel = calculate_fuel_part_2(&crabs, crabs[0]);

	for pos in (crabs[0] + 1)..(*crabs.last().unwrap() + 1) {
		let fuel = calculate_fuel_part_2(&crabs, pos);
		if fuel < min_fuel {
			min_fuel = fuel;
		} else {
			break;
		}
	}

	min_fuel
}
