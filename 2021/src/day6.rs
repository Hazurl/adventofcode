use crate::utility;
use itertools::*;
use std::slice::Iter;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<u8> {
	utility::read_lines(format!("{}/day6/{}.txt", base_path_inputs, filename))[0]
		.split(',')
		.map(|n| n.parse().unwrap())
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("6.1.example", &part1, &input_example);
	utility::benchmark("6.1", &part1, &input);

	utility::benchmark("6.2.example", &part2, &input_example);
	utility::benchmark("6.2", &part2, &input);
}

fn simulate(initial_population: &Vec<u8>, max_gen: usize) -> u64 {
	let mut population = [0u64; 9];

	for n in initial_population {
		population[*n as usize] += 1;
	}

	for gen in 0..max_gen {
		let mut new_population = [0u64; 9];

		new_population[8] = population[0]; // newborn
		new_population[7] = population[8];
		new_population[6] = population[7] + population[0]; // newborn becoming mature + adult that created a child
		new_population[5] = population[6];
		new_population[4] = population[5];
		new_population[3] = population[4];
		new_population[2] = population[3];
		new_population[1] = population[2];
		new_population[0] = population[1];

		population = new_population;
	}

	population.iter().sum()
}

fn part1(initial_population: &Vec<u8>) -> u64 {
	simulate(initial_population, 80)
}

fn part2(initial_population: &Vec<u8>) -> u64 {
	simulate(initial_population, 256)
}
