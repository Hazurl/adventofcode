use crate::utility;

fn parse_file(base_path_inputs: &str, filename: &str) -> std::vec::Vec<String> {
	utility::read_lines(format!("{}/day3/{}.txt", base_path_inputs, filename))
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("2.1.example", &part1, &input_example);
	utility::benchmark("2.1", &part1, &input);

	utility::benchmark("2.2.example", &part2, &input_example);
	utility::benchmark("2.2", &part2, &input);
}

fn part1(bins: &std::vec::Vec<String>) -> u64 {
	let input_size = bins[0].len();
	let gamma: u64 = (0..input_size)
		.zip(std::iter::repeat(bins.into_iter()))
		.map(|(index, it)| {
			let count1 = it
				.map(|v| v.chars().nth(index).unwrap())
				.filter(|c| *c == '1')
				.count();
			(index, (count1 * 2 >= bins.len()) as u64)
		})
		.map(|(index, digit)| digit << (input_size - index - 1))
		.sum();

	gamma * (!gamma & (1 << input_size) - 1)
}

fn rating_from_bit_criteria<F: Fn(usize, usize) -> char>(
	bins: std::vec::Vec<String>,
	bit_criteria: F,
) -> u64 {
	let input_size = bins[0].len();
	(0..input_size).fold(bins.clone(), |values, index| {
		if values.len() == 1 {
			return values;
		}
		let count1 = values
			.iter()
			.map(|v| v.chars().nth(index).unwrap())
			.filter(|c| *c == '1')
			.count();

		let to_keep = bit_criteria(count1, values.len());
		values
			.iter()
			.filter(|v| v.chars().nth(index).unwrap() == to_keep)
			.cloned()
			.collect()
	})[0]
		.chars()
		.enumerate()
		.map(|(index, digit)| (digit.to_digit(10).unwrap() as u64) << (input_size - index - 1))
		.sum()
}

fn part2(bins: &std::vec::Vec<String>) -> u64 {
	rating_from_bit_criteria(
		bins.clone(),
		|count1, size| {
			if count1 * 2 >= size {
				'1'
			} else {
				'0'
			}
		},
	) * rating_from_bit_criteria(
		bins.clone(),
		|count1, size| {
			if count1 * 2 < size {
				'1'
			} else {
				'0'
			}
		},
	)
}
