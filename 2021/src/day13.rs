use crate::utility;
use std::vec::Vec;
use std::collections::HashSet;

#[derive(Clone, Copy, Debug)]
enum Fold {
	Horizontal(i64),
	Vertical(i64),
}

#[derive(Clone, Debug)]
struct Input {
	points: Vec<(i64, i64)>,
	folds: Vec<Fold>,
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Input {
	let mut input = Input{ points: Vec::new(), folds: Vec::new() };
	let content = utility::read_lines(format!("{}/day13/{}.txt", base_path_inputs, filename));

	for line in content {
		if line.chars().nth(0).unwrap() == 'f' {
			let mut i = line.split('=');
			i.next();
			if line.contains('x') {
				input.folds.push(Fold::Vertical(i.next().unwrap().parse().unwrap()));
			} else {
				input.folds.push(Fold::Horizontal(i.next().unwrap().parse().unwrap()));
			}
		} else {
			let mut nums = line.split(',');
			input.points.push((nums.next().unwrap().parse().unwrap(), nums.next().unwrap().parse().unwrap()));
		}
	}

	input
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	println!("{:?}", input_example);

	utility::benchmark("13.1.example", &part1, input_example.clone());
	utility::benchmark("13.1", &part1, input.clone());

	utility::benchmark("13.2.example", &part2, input_example);
	utility::benchmark("13.2", &part2, input);
}

fn fold_point(p: (i64, i64), fold: Fold) -> (i64, i64) {
	match fold {
		Fold::Vertical(x) => (if p.0 < x { p.0 } else { x - (p.0 - x) }, p.1),
		Fold::Horizontal(y) => (p.0, if p.1 < y { p.1 } else { y - (p.1 - y) }),
	}
}

fn part1(input: Input) -> u64 {
	let mut set = HashSet::new();
	let fold = input.folds[0];
	for p in input.points {
		set.insert(fold_point(p, fold));
	}

	set.len() as u64
}

fn do_fold(set: &HashSet<(i64, i64)>, fold: Fold) -> HashSet<(i64, i64)> {
	let mut next_set = HashSet::new();
	for p in set {
		next_set.insert(fold_point(*p, fold));
	}

	next_set
}

fn part2(input: Input) -> u64 {
	let mut set = input.points.iter().copied().collect();
	for f in input.folds {
		set = do_fold(&set, f);
	}

	let mut width = 0;
	let mut height = 0;
	for p in &set {
		assert!(p.0 >= 0);
		assert!(p.1 >= 0);
		width = if p.0 >= width { p.0 + 1 } else { width };
		height = if p.1 >= height { p.1 + 1 } else { height };
	}

	let mut grid = vec![false; (width * height) as usize];
	for p in &set {
		grid[(p.0 + p.1 * width) as usize] = true;
	}

	for y in 0..height {
		for x in 0..width {
			let idx = x + y * width;
			print!("{}", if grid[idx as usize] { "##" } else { "  " });
		}
		println!("");
	}
	println!("");

	0
}
