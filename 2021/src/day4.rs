use crate::utility;
use itertools::*;
use std::slice::Iter;
use std::vec::Vec;

#[derive(Debug, Clone)]
struct Board(Vec<Vec<(u32, bool)>>);

#[derive(Debug, Clone)]
struct Game {
	draws: Vec<u32>,
	boards: Vec<Board>,
}

fn parse_draws(line: &String) -> Vec<u32> {
	line.split(',').map(|n| n.parse().unwrap()).collect()
}

fn parse_board(chunk: Chunk<'_, Iter<'_, String>>) -> Board {
	Board(
		chunk
			.map(|line| {
				line
					.split(' ')
					.filter(|n| n.len() > 0)
					.map(|n| (n.parse().unwrap(), false))
					.collect()
			})
			.collect(),
	)
}

fn parse_boards(it: Iter<'_, String>) -> Vec<Board> {
	it.chunks(5).into_iter().map(&parse_board).collect()
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Game {
	let content = utility::read_lines(format!("{}/day4/{}.txt", base_path_inputs, filename));
	Game {
		draws: parse_draws(&content[0]),
		boards: parse_boards(content[1..].iter()),
	}
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");

	utility::benchmark("2.1.example", &part1, input_example.clone());
	utility::benchmark("2.1", &part1, input.clone());

	utility::benchmark("2.2.example", &part2, input_example);
	utility::benchmark("2.2", &part2, input);
}

fn part1(mut game: Game) -> u64 {
	for draw in game.draws {
		for board in &mut game.boards {
			let mut marked_element: Option<(usize, usize)> = None;
			'board_loop: for (row_idx, line) in board.0.iter_mut().enumerate() {
				for (col_idx, elem) in line.iter_mut().enumerate() {
					if elem.0 == draw {
						elem.1 = true;
						marked_element = Some((row_idx, col_idx));
						break 'board_loop;
					}
				}
			}

			if let Some((row_idx, col_idx)) = marked_element {
				let line_done = board.0[row_idx].iter().all(|(_, marked)| *marked);
				let col_done = board.0.iter().all(|row| row[col_idx].1);
				if line_done || col_done {
					return calculate_score(board, draw);
				}
			}
		}
	}
	0
}

fn calculate_score(board: &Board, last_draw: u32) -> u64 {
	let mut sum = 0u32;
	for line in &board.0 {
		for elem in line {
			if !elem.1 {
				sum += elem.0;
			}
		}
	}
	return (sum * last_draw) as u64;
}

fn part2(_game: Game) -> u64 {
	0
}
