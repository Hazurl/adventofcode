use crate::utility;
use itertools::*;

fn parse_file(base_path_inputs: &str, filename: &str) -> std::vec::Vec<String> {
	utility::read_lines(format!("{}/day2/{}.txt", base_path_inputs, filename))
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("2.1.example", &part1, &input_example);
	utility::benchmark("2.1", &part1, &input);

	utility::benchmark("2.2.example", &part2, &input_example);
	utility::benchmark("2.2", &part2, &input);
}

fn part1(cmds: &std::vec::Vec<String>) -> i64 {
	let (horizontal, depth) = cmds.into_iter().fold((0, 0), |(horizontal, depth), cmd| {
		let arg = cmd
			.chars()
			.skip_while(|c| *c != ' ')
			.skip(1)
			.collect::<String>()
			.parse::<i64>()
			.unwrap();

		match cmd.chars().nth(0).unwrap() {
			'f' => (horizontal + arg, depth),
			'u' => (horizontal, depth - arg),
			'd' => (horizontal, depth + arg),
			_ => panic!("Unknown command"),
		}
	});

	horizontal * depth
}

fn part2(cmds: &std::vec::Vec<String>) -> i64 {
	let (horizontal, depth, aim) =
		cmds
			.into_iter()
			.fold((0, 0, 0), |(horizontal, depth, aim), cmd| {
				let arg = cmd
					.chars()
					.skip_while(|c| *c != ' ')
					.skip(1)
					.collect::<String>()
					.parse::<i64>()
					.unwrap();

				match cmd.chars().nth(0).unwrap() {
					'f' => (horizontal + arg, depth + aim * arg, aim),
					'u' => (horizontal, depth, aim - arg),
					'd' => (horizontal, depth, aim + arg),
					_ => panic!("Unknown command"),
				}
			});

	horizontal * depth
}
