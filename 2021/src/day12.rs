use crate::utility;
use std::vec::Vec;
use std::collections::HashMap;
use std::collections::HashSet;

fn parse_line(str: &String) -> (String, String) {
	let mut split = str.split('-');
	let from = split.next().unwrap().to_string();
	let to = split.next().unwrap().to_string();

	(from, to)
}

fn add_line_to_graph(mut graph: HashMap<String, Vec<String>>, line: &String) -> HashMap<String, Vec<String>> {
	let (from, to) = parse_line(line);
	if !graph.contains_key(&from) {
		graph.insert(from.clone(), Vec::new());
	}
	graph.get_mut(&from).unwrap().push(to.clone());

	if !graph.contains_key(&to) {
		graph.insert(to.clone(), Vec::new());
	}
	graph.get_mut(&to).unwrap().push(from.clone());

	graph
}

fn parse_file(base_path_inputs: &str, filename: &str) -> HashMap<String, Vec<String>> {
	utility::read_lines(format!("{}/day12/{}.txt", base_path_inputs, filename)).iter().fold(HashMap::new(), &add_line_to_graph)
}

pub fn run(base_path_inputs: &str) {
	let input = parse_file(base_path_inputs, "input");
	let input_example = parse_file(base_path_inputs, "example");
	// println!("{:?}", input_example);

	utility::benchmark("10.1.example", &part1, input_example.clone());
	utility::benchmark("10.1", &part1, input.clone());

	utility::benchmark("10.2.example", &part2, input_example);
	utility::benchmark("10.2", &part2, input);
}

fn is_small_cave(cave: &String) -> bool {
	let c = cave.chars().nth(0).unwrap();
	c >= 'a' && c <= 'z'
}

fn all_path(graph: &HashMap<String, Vec<String>>, mut small_caves_visited: &mut HashSet<String>, current_cave: &String) -> u64 {
	if current_cave == "end" {
		return 1;
	}
	let mut path_count = 0;
	for next_cave in &graph[current_cave] {
		if is_small_cave(next_cave) {
			if !small_caves_visited.contains(next_cave) {
				small_caves_visited.insert(next_cave.clone());
				path_count += all_path(graph, small_caves_visited, next_cave);
				small_caves_visited.remove(next_cave);
			}
		} else {
			path_count += all_path(graph, small_caves_visited, next_cave);
		}
	}

	path_count
}

fn part1(graph: HashMap<String, Vec<String>>) -> u64 {
	let mut small_caves_visited = HashSet::<String>::new();
	let start = "start".to_string();

	small_caves_visited.insert(start.clone());
	all_path(&graph, &mut small_caves_visited, &start)
}

fn all_path_2(graph: &HashMap<String, Vec<String>>, mut small_caves_visited_once: &mut HashSet<String>, mut small_cave_visited_twice: &mut bool, current_cave: &String) -> u64 {
	if current_cave == "end" {
		return 1;
	}
	let mut path_count = 0;
	for next_cave in &graph[current_cave] {
		if next_cave == "start" {
			continue;
		}

		if is_small_cave(next_cave) {
			if !small_caves_visited_once.contains(next_cave) {
				small_caves_visited_once.insert(next_cave.clone());
				path_count += all_path_2(graph, small_caves_visited_once, small_cave_visited_twice, next_cave);
				small_caves_visited_once.remove(next_cave);
			} else if !*small_cave_visited_twice {
				*small_cave_visited_twice = true;
				path_count += all_path_2(graph, small_caves_visited_once, small_cave_visited_twice, next_cave);
				*small_cave_visited_twice = false;
			}
		} else {
			path_count += all_path_2(graph, small_caves_visited_once, small_cave_visited_twice, next_cave);
		}
	}

	path_count
}

fn part2(graph: HashMap<String, Vec<String>>) -> u64 {
	let mut small_caves_visited_once = HashSet::<String>::new();
	let mut small_cave_visited_twice = false;
	let start = "start".to_string();
	all_path_2(&graph, &mut small_caves_visited_once, &mut small_cave_visited_twice, &start)
}
