use crate::utility;
use std::collections::BinaryHeap;
use std::vec::Vec;

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<String> {
	utility::read_lines(format!("{}/day18/{}.txt", base_path_inputs, filename))
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("18.1.example", &part1, &input_example);
	utility::benchmark("18.1", &part1, &input);

	utility::benchmark("18.2.example", &part2, &input_example);
	utility::benchmark("18.2", &part2, &input);
}

// Instead of a having a recursive data structure, each elements has a depth and a value
// It is also reversed, because splitting and exploding occurs on the first elements first,
// but might requires all the next elements to shift indices
// This also means you can cache all the exploding pairs, since their indices won't change

// For example:
// [[0, 1], 2]
// is
// [(1, 0), (1, 1), (0, 2)]
// This allows to find easily the adjacent elements and cut down the number of allocations needed
type SnailFishNumber = Vec<(usize, u64)>;

fn parse(str: &String) -> SnailFishNumber {
	let mut num = SnailFishNumber::new();
	let mut depth = 0;

	for c in str.chars() {
		match c {
			'[' => depth += 1,
			']' => depth -= 1,
			',' => {}
			'\n' => {}
			'\r' => {}
			d => num.push((depth - 1, d.to_digit(10).unwrap() as u64)),
		}
	}

	num.reverse();

	num
}

fn add_to(mut lhs: SnailFishNumber, mut rhs: SnailFishNumber) -> SnailFishNumber {
	let mut to_explode = Vec::new();

	// since it's reversed, we append lhs to rhs to have [lhs..., rhs...]
	rhs.append(&mut lhs);

	let mut push_to_explode = true;
	for (idx, elem) in (&mut rhs).into_iter().enumerate() {
		elem.0 += 1; // [lhs..., rhs...] means every depth increase by one
		if elem.0 >= 4 {
			if push_to_explode {
				to_explode.push(idx);
			}
			push_to_explode = !push_to_explode; // we only push one of the element of the pair
		}
	}

	to_explode.reverse();

	// Exploding => [a, [b, c], d] => [a+b, 0, c+d]
	for idx in to_explode {
		if idx < rhs.len() - 2 {
			rhs[idx + 2].1 += rhs[idx + 1].1;
		}

		if idx > 0 {
			rhs[idx - 1].1 += rhs[idx].1;
		}

		rhs[idx + 1] = (rhs[idx + 1].0 - 1, 0);
		rhs.remove(idx);
	}

	// Try to split until there is no more
	// Spliting add at most one pair to explode
	// So instead of checking pairs to explode between each split, we merge the split and the explosion when a split generate a pair to explode
	let mut ridx = 0;
	while ridx < rhs.len() {
		let idx = rhs.len() - ridx - 1; // Starts from the end (= starts of the number since it's reversed)
		ridx += 1;
		let (depth, value) = rhs[idx];

		if value >= 10 {
			let left = value / 2;
			let right = (value + 1) / 2;

			// Split+Explode => [a, b, c] (b >= 10) => [a, [b/2, (b + 1)/2], c] => [a+b/2, 0, c+(b+1)/2]
			// This is a shortcut to avoid having to move all the other elements, as this combination keep the same number of elements
			if depth + 1 >= 4 {
				if idx < rhs.len() - 1 {
					rhs[idx + 1].1 += left;
				}
				rhs[idx] = (depth, 0);
				if idx > 0 {
					rhs[idx - 1].1 += right;
				}
				ridx = 0;
			} else {
				// Split without explosion => [a, b, c] (b >= 10) => [a, [b/2, (b + 1)/2], c]
				rhs.insert(idx + 1, (depth + 1, left));
				rhs[idx] = (depth + 1, right);
				if right >= 10 {
					ridx -= 1; // if b/2 >= 10
				}
			}
		}
	}

	rhs
}

fn magnitude_of_helper(num: &SnailFishNumber, mut idx: &mut usize, depth: usize) -> u64 {
	let left = if num[*idx].0 == depth {
		*idx += 1;
		num[*idx - 1].1
	} else {
		magnitude_of_helper(&num, &mut idx, depth + 1)
	};

	let right = if num[*idx].0 == depth {
		*idx += 1;
		num[*idx - 1].1
	} else {
		magnitude_of_helper(&num, &mut idx, depth + 1)
	};

	left * 2 + right * 3
}

fn magnitude_of(num: &SnailFishNumber) -> u64 {
	magnitude_of_helper(&num, &mut 0, 0)
}

fn part1(input: &Vec<String>) -> u64 {
	let mut sum = parse(&input[0]);

	for line in input.into_iter().skip(1) {
		let num = parse(&line);
		sum = add_to(sum, num);
	}

	magnitude_of(&sum)
}

fn part2(input: &Vec<String>) -> u64 {
	let nums = input.into_iter().map(&parse).collect::<Vec<_>>();

	let mut max_mag = 0;

	for lhs in &nums {
		for rhs in &nums {
			max_mag = max_mag.max(magnitude_of(&add_to(lhs.clone(), rhs.clone())));
		}
	}

	max_mag
}
