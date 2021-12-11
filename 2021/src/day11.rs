use crate::utility;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<Vec<char>> {
	utility::read_lines(format!("{}/day11/{}.txt", base_path_inputs, filename))
		.into_iter()
		.map(|l| 
			l.chars()
			.filter(|c| *c >= '0' && *c <= '9')
			.collect())
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("10.1.example", &part1, input_example.clone());
	utility::benchmark("10.1", &part1, input.clone());

	utility::benchmark("10.2.example", &part2, input_example);
	utility::benchmark("10.2", &part2, input);
}

fn perform_flash(x: usize, y: usize, mut grid: &mut Vec<Vec<char>>) -> u64 {
	grid[x][y] = '0';
	let mut flash_count = 1;

	let minx = if x == 0 { 0 } else { x - 1 };
	let miny = if y == 0 { 0 } else { y - 1 };
	let maxx = if x == grid.len() - 1 { x } else { x + 1 };
	let maxy = if y == grid[x].len() - 1 { y } else { y + 1 };

	for rx in minx..(maxx + 1) {
		for ry in miny..(maxy + 1) {
			if rx != x || ry != y {
				if grid[rx][ry] > '0' {
					 grid[rx][ry] = (grid[rx][ry] as u8 + 1) as char;
					if grid[rx][ry] > '9' {
						flash_count += perform_flash(rx, ry, &mut grid);
					}
				}
			}
		}
	}

	flash_count
}

fn step(mut grid: &mut Vec<Vec<char>>) -> u64 {
	for x in 0..grid.len() {
		for y in 0..grid[x].len() {
			grid[x][y] = (grid[x][y] as u8 + 1) as char;
		}
	}

	let mut flash = 0;

	for x in 0..grid.len() {
		for y in 0..grid[x].len() {
			let o = grid[x][y];
			if o > '9' {
				flash += perform_flash(x, y, &mut grid);
			}
		}
	}

	flash
}

fn part1(mut grid: Vec<Vec<char>>) -> u64 {
	let mut flash_count = 0;
	for _ in 0..100 {
		flash_count += step(&mut grid);
	}

	flash_count
}

fn part2(mut grid: Vec<Vec<char>>) -> u64 {
	for i in 0.. {
		let flash_count = step(&mut grid);
		if flash_count == (grid.len() * grid[0].len()) as u64 {
			return i + 1; 
		}
	}

	0
}
