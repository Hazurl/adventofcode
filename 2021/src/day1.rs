use crate::utility;
use itertools::*;

fn parse_file(base_path_inputs: &str, filename: &str) -> std::vec::Vec<i64> {
	utility::read_lines(format!("{}/day1/{}.txt", base_path_inputs, filename))
		.iter()
		.map(|line| line.parse::<i64>().unwrap())
		.collect()
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("1.1.example", &part1, &input_example);
	utility::benchmark("1.1", &part1, &input);

	utility::benchmark("1.2.example", &part2, &input_example);
	utility::benchmark("1.2", &part2, &input);
}

fn part1(scans: &std::vec::Vec<i64>) -> i64 {
	scans
		.into_iter()
		.tuple_windows()
		.filter(|(prev, next)| next > prev)
		.count() as i64
}

fn part2(scans: &std::vec::Vec<i64>) -> i64 {
	scans
		.into_iter()
		.tuple_windows()
		.map(|(a, b, c)| a + b + c)
		.tuple_windows()
		.filter(|(prev, next)| next > prev)
		.count() as i64
}
