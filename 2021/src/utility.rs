pub fn read_lines<P: AsRef<std::path::Path>>(path: P) -> std::vec::Vec<String> {
	std::fs::read_to_string(path)
		.unwrap()
		.split('\n')
		.filter(|line| line.len() > 0)
		.map(|line| line.to_string())
		.collect()
}

pub fn benchmark<I, O: std::fmt::Display, F: FnOnce(I) -> O>(title: &str, f: F, input: I) {
	let begin = std::time::Instant::now();

	let result = f(input);

	let duration = begin.elapsed().as_micros();

	println!("{}: {} in {}ms", title, result, duration as f64 / 1000.0);
}
