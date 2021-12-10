use crate::utility;
// use regex::Regex;
// use std::collections::HashMap;
use std::collections::HashSet;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<Vec<u32>> {
	utility::read_lines(format!("{}/day9/{}.txt", base_path_inputs, filename))
		.into_iter()
		.map(|line| line
			.chars()
			.filter(|c| *c >= '0' && *c <= '9')
			.map(|c|  
				c.to_digit(10)
				.unwrap())
			.collect::<Vec<u32>>())
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("9.1.example", &part1, input_example.clone());
	utility::benchmark("9.1", &part1, input.clone());

	utility::benchmark("9.2.example", &part2, input_example);
	utility::benchmark("9.2", &part2, input);
}

fn is_minimum(field: &Vec<Vec<u32>>, x: usize, y: usize) -> bool {
	let value = field[x][y];
	if x > 0 {
		if field[x - 1][y] <= value {
			return false;
		}
	}

	if x < field.len() - 1 {
		if field[x + 1][y] <= value {
			return false;
		}
	}

	if y > 0 {
		if field[x][y - 1] <= value {
			return false;
		}
	}

	if y < field[x].len() - 1 {
		if field[x][y + 1] <= value {
			return false;
		}
	}

	return true;
}

fn part1(field: Vec<Vec<u32>>) -> u64 {
	let mut sum = 0;
	for x in 0..(field.len()) {
		for y in 0..(field[x].len()) {
			if is_minimum(&field, x, y) {
				sum += 1 + field[x][y];
			}
		}
	}

	sum as u64
}

fn get_bassins_size(field: &Vec<Vec<u32>>, x: usize, y: usize) -> u64 {
	let mut done = HashSet::<(usize, usize)>::new();
	let mut to_visit = Vec::<(usize, usize)>::new();

	to_visit.push((x, y));
	done.insert((x, y));

	while to_visit.len() > 0 {
		let visiting = to_visit.last().unwrap().clone();
		to_visit.pop();

		if visiting.0 > 0 {
			if field[visiting.0 - 1][visiting.1] < 9 {
				if !done.contains(&(visiting.0 - 1, visiting.1)) {
					done.insert((visiting.0 - 1, visiting.1));
					to_visit.push((visiting.0 - 1, visiting.1));
				}
			}
		}
	
		if visiting.0 < field.len() - 1 {
			if field[visiting.0 + 1][visiting.1] < 9 {
				if !done.contains(&(visiting.0 + 1, visiting.1)) {
					done.insert((visiting.0 + 1, visiting.1));
					to_visit.push((visiting.0 + 1, visiting.1));
				}
			}
		}
	
		if visiting.1 > 0 {
			if field[visiting.0][visiting.1 - 1] < 9 {
				if !done.contains(&(visiting.0, visiting.1 - 1)) {
					done.insert((visiting.0, visiting.1 - 1));
					to_visit.push((visiting.0, visiting.1 - 1));
				}
			}
		}
	
		if visiting.1 < field[visiting.0].len() - 1 {
			if field[visiting.0][visiting.1 + 1] < 9 {
				if !done.contains(&(visiting.0, visiting.1 + 1)) {
					done.insert((visiting.0, visiting.1 + 1));
					to_visit.push((visiting.0, visiting.1 + 1));
				}
			}
		}
	}

	println!("{:?} from {:?}", done, (x, y));

	done.len() as u64
}

fn part2(field: Vec<Vec<u32>>) -> u64 {
	let mut bassins = Vec::new();
	for x in 0..(field.len()) {
		for y in 0..(field[x].len()) {
			if is_minimum(&field, x, y) {
				bassins.push(get_bassins_size(&field, x, y));
			}
		}
	}

	bassins.sort();
	let len = bassins.len();
	println!("{:?}", bassins);
	return bassins[len - 1] * bassins[len - 2] * bassins[len - 3];
}
