use crate::utility;
use itertools::*;
use std::slice::Iter;
use std::vec::Vec;

#[derive(Clone, Debug)]
struct Point(usize, usize);

#[derive(Clone, Debug)]
struct Segment {
	start: Point,
	end: Point,
}

fn parse_segment(str: &String) -> Segment {
	let v = str
		.split(" -> ")
		.map(|s| {
			let v = s.split(',').map(|n| n.parse().unwrap()).collect::<Vec<_>>();
			Point(v[0], v[1])
		})
		.collect::<Vec<_>>();

	Segment {
		start: v[0].clone(),
		end: v[1].clone(),
	}
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<Segment> {
	utility::read_lines(format!("{}/day5/{}.txt", base_path_inputs, filename))
		.iter()
		.map(&parse_segment)
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("2.1.example", &part1, &input_example);
	utility::benchmark("2.1", &part1, &input);

	utility::benchmark("2.2.example", &part2, &input_example);
	utility::benchmark("2.2", &part2, &input);
}

fn inc_cell(grid: &mut Vec<Vec<u8>>, point: Point) -> u8 {
	if grid.len() <= point.0 {
		grid.resize(point.0 + 1, Vec::new());
	}

	if grid[point.0].len() <= point.1 {
		grid[point.0].resize(point.1 + 1, 0);
	}

	grid[point.0][point.1] += 1;
	grid[point.0][point.1]
}

fn range(from: usize, to: usize) -> std::ops::Range<usize> {
	if from < to {
		from..(to + 1)
	} else {
		to..(from + 1)
	}
}

fn abs_diff(a: usize, b: usize) -> usize {
	if a < b {
		b - a
	} else {
		a - b
	}
}

fn cell_from_abs_range(from: usize, to: usize, d: usize) -> usize {
	if from < to {
		from + d
	} else {
		from - d
	}
}

fn part1(segments: &Vec<Segment>) -> u64 {
	let mut grid = Vec::<Vec<u8>>::new();
	let mut count_overlap = 0;

	for segment in segments {
		if segment.start.0 == segment.end.0 {
			for y in range(segment.start.1, segment.end.1) {
				if inc_cell(&mut grid, Point(segment.start.0, y)) == 2 {
					count_overlap += 1;
				}
			}
		}

		if segment.start.1 == segment.end.1 {
			for x in range(segment.start.0, segment.end.0) {
				if inc_cell(&mut grid, Point(x, segment.start.1)) == 2 {
					count_overlap += 1;
				}
			}
		}
	}

	count_overlap
}

fn part2(segments: &Vec<Segment>) -> u64 {
	let mut grid = Vec::<Vec<u8>>::new();
	let mut count_overlap = 0;

	for segment in segments {
		if segment.start.0 == segment.end.0 {
			for y in range(segment.start.1, segment.end.1) {
				if inc_cell(&mut grid, Point(segment.start.0, y)) == 2 {
					count_overlap += 1;
				}
			}
		}

		if segment.start.1 == segment.end.1 {
			for x in range(segment.start.0, segment.end.0) {
				if inc_cell(&mut grid, Point(x, segment.start.1)) == 2 {
					count_overlap += 1;
				}
			}
		}

		if abs_diff(segment.start.0, segment.end.0) == abs_diff(segment.start.1, segment.end.1) {
			for d in range(0, abs_diff(segment.start.0, segment.end.0)) {
				let x = cell_from_abs_range(segment.start.0, segment.end.0, d);
				let y = cell_from_abs_range(segment.start.1, segment.end.1, d);
				if inc_cell(&mut grid, Point(x, y)) == 2 {
					count_overlap += 1;
				}
			}
		}
	}

	count_overlap
}
