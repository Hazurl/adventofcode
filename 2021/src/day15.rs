use crate::utility;
use std::collections::BTreeMap;
use std::collections::HashMap;
use std::collections::HashSet;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<Vec<u8>> {
	utility::read_lines(format!("{}/day15/{}.txt", base_path_inputs, filename))
		.iter()
		.map(|line| {
			line
				.chars()
				.map(|n| n.to_digit(10).unwrap() as u8)
				.collect()
		})
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("15.1.example", &part1, input_example.clone());
	utility::benchmark("15.1", &part1, input.clone());

	utility::benchmark("15.2.example", &part2, input_example);
	utility::benchmark("15.2", &part2, input);
}

fn estimated_cost_of((width, height): (usize, usize), (x, y): (usize, usize)) -> u64 {
	return ((width - 1 - x) + (height - 1 - y)) as u64;
}

fn neighbors_of((width, height): (usize, usize), (x, y): (usize, usize)) -> Vec<(usize, usize)> {
	let mut neighbors = Vec::new();
	if x > 0 {
		neighbors.push((x - 1, y));
	}

	if x < width - 1 {
		neighbors.push((x + 1, y));
	}

	if y > 0 {
		neighbors.push((x, y - 1));
	}

	if y < height - 1 {
		neighbors.push((x, y + 1));
	}

	neighbors
}

fn next_to_visit(to_visit: &HashMap<(usize, usize), (u64, u64)>) -> ((usize, usize), u64) {
	let (visiting, (cost, _)) = to_visit
		.iter()
		.min_by(|(_, (cost1, estimated1)), (_, (cost2, estimated2))| {
			(cost1 + estimated1).cmp(&(cost2 + estimated2))
		})
		.unwrap();

	(*visiting, *cost)
}

fn get_weight(grid: &Vec<Vec<u8>>, (x, y): (usize, usize)) -> u64 {
	let tx = x / grid.len();
	let rx = x % grid.len();

	let ty = y / grid[0].len();
	let ry = y % grid[0].len();

	let w = grid[rx][ry] as u64 + tx as u64 + ty as u64;
	(w - 1) % 9 + 1
}

fn cost_finding(grid: &Vec<Vec<u8>>, size: (usize, usize)) -> u64 {
	let mut to_visit = HashMap::<(usize, usize), (u64, u64)>::new(); // (x, y) -> (Cost from start, Remaining estimated cost)
	let mut parents = HashMap::new();
	let mut visited = HashSet::new();

	parents.insert((0, 0), (0, 0));
	to_visit.insert((0, 0), (0, estimated_cost_of(size, (0, 0))));

	while !to_visit.is_empty() {
		let (visiting, cost) = next_to_visit(&to_visit);
		to_visit.remove(&visiting);
		visited.insert(visiting);

		if visiting == (size.0 - 1, size.1 - 1) {
			return cost;
		}

		for n in neighbors_of(size, visiting) {
			if visited.contains(&n) {
				continue;
			}

			let weight = get_weight(&grid, n);
			let total_cost = weight + cost;

			if !to_visit.contains_key(&n) || to_visit.get(&n).unwrap().0 > total_cost {
				to_visit.insert(n, (total_cost, estimated_cost_of(size, n)));
				parents.insert(n, visiting);
			}
		}
	}

	0
}

fn part1(grid: Vec<Vec<u8>>) -> u64 {
	cost_finding(&grid, (grid.len(), grid[0].len()))
}

fn part2(grid: Vec<Vec<u8>>) -> u64 {
	cost_finding(&grid, (grid.len() * 5, grid[0].len() * 5))
}
