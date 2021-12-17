use crate::utility;
use regex::Regex;
use std::vec::Vec;

#[derive(Clone, Copy, Eq, PartialEq, Debug)]
struct Target {
	x: (i64, i64),
	y: (i64, i64),
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Target {
	let line = utility::read_lines(format!("{}/day17/{}.txt", base_path_inputs, filename))[0].clone();
	let regex = Regex::new(r"target area: x=([-0-9]+)..([-0-9]+), y=([-0-9]+)..([-0-9]+)").unwrap();

	let captures = regex.captures(&line).unwrap();

	Target {
		x: (captures[1].parse().unwrap(), captures[2].parse().unwrap()),
		y: (captures[3].parse().unwrap(), captures[4].parse().unwrap()),
	}
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	println!("{:?}", input);
	println!("{:?}", input_example);

	utility::benchmark("17.1.example", &part1, input_example.clone());
	utility::benchmark("17.1", &part1, input.clone());

	utility::benchmark("17.2.example", &part2, input_example);
	utility::benchmark("17.2", &part2, input);
}

fn in_between(a: i64, range: (i64, i64)) -> bool {
	range.0 <= a && a <= range.1
}

fn max_height(vy: i64) -> i64 {
	(vy * (vy + 1)) / 2
}

fn will_hit_target(mut vx: i64, mut vy: i64, target: &Target) -> bool {
	let mut x = 0;
	let mut y = 0;

	loop {
		x += vx;
		y += vy;

		if vx > 0 {
			vx -= 1;
		}

		vy -= 1;

		if in_between(y, target.y) && in_between(x, target.x) {
			return true;
		}

		if y < target.y.0 {
			return false;
		}
	}
}

fn part1(target: Target) -> u64 {
	max_height(-target.y.0 - 1) as u64
}

fn part2(target: Target) -> u64 {
	let mut min_x = 1;
	let mut x = 0;

	while x < target.x.0 {
		min_x += 1;
		x += min_x;
	}

	let max_x = target.x.1;
	let max_y = -target.y.0 - 1;
	let min_y = target.y.0;

	let mut count = 0;
	for vx in min_x..(max_x + 1) {
		for vy in min_y..(max_y + 1) {
			let hit = will_hit_target(vx, vy, &target);
			count += hit as u64;
		}
	}

	count
}
