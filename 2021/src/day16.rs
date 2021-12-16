use crate::utility;
use std::vec::Vec;

#[derive(Clone, Eq, PartialEq, Debug)]
enum TypeID {
	Literal(u64),
	Op(u8, Vec<Packet>),
}

#[derive(Clone, Eq, PartialEq, Debug)]
struct Packet {
	version: u8,
	value: TypeID,
}

#[derive(Clone, Debug)]
struct Reader<'a> {
	slice: &'a [bool],
	index: usize,
	max_length: usize,
}

fn parse_file(base_path_inputs: &str, filename: &str) -> Vec<bool> {
	utility::read_lines(format!("{}/day16/{}.txt", base_path_inputs, filename))[0]
		.chars()
		.map(|n| n.to_digit(16).unwrap() as u8)
		.flat_map(|n| [n & 0b1000 != 0, n & 0b100 != 0, n & 0b10 != 0, n & 0b1 != 0])
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

fn read_bit(mut reader: &mut Reader) -> bool {
	if is_eof(reader) {
		false
	} else {
		reader.index += 1;
		reader.slice[reader.index - 1]
	}
}

fn is_eof(reader: &Reader) -> bool {
	reader.index >= reader.max_length
}

fn take<'a>(reader: &Reader<'a>, size: usize) -> Reader<'a> {
	Reader {
		slice: reader.slice,
		index: reader.index,
		max_length: reader.max_length.min(reader.index + size),
	}
}

fn read_number(mut reader: &mut Reader, mut size: usize) -> u64 {
	let mut num = 0;

	while size > 0 {
		size -= 1;
		num = (num << 1) | (read_bit(&mut reader) as u64);
	}

	num
}

fn parse_literal(mut reader: &mut Reader) -> TypeID {
	let mut num = 0;

	loop {
		let n = read_number(&mut reader, 5);
		num = (num << 4) | n & 0xF;

		if n & 0b10000 == 0 {
			break;
		}
	}

	TypeID::Literal(num)
}

fn parse_op(mut reader: &mut Reader, op: u8) -> TypeID {
	let length_id = read_number(&mut reader, 1);

	let mut subpackets = Vec::new();

	match length_id {
		0 => {
			let size = read_number(&mut reader, 15) as usize;
			let mut subpackets_reader = take(reader, size);
			reader.index += size;
			while !is_eof(&subpackets_reader) {
				subpackets.push(parse_packet(&mut subpackets_reader));
			}
		}
		1 => {
			let mut size = read_number(&mut reader, 11);
			while size > 0 {
				size -= 1;
				subpackets.push(parse_packet(&mut reader));
			}
		}
		_ => {
			unreachable!()
		}
	}

	TypeID::Op(op, subpackets)
}

fn parse_packet(mut reader: &mut Reader) -> Packet {
	let version = read_number(&mut reader, 3) as u8;
	let typeid = read_number(&mut reader, 3) as u8;

	let value = match typeid {
		4 => parse_literal(&mut reader),
		op => parse_op(&mut reader, op),
	};

	Packet {
		version,
		value: value,
	}
}

fn sum_of_version(packet: &Packet) -> u64 {
	packet.version as u64
		+ match &packet.value {
			TypeID::Literal(_) => 0,
			TypeID::Op(_, subpackets) => subpackets.iter().map(&sum_of_version).sum(),
		}
}

fn part1(input: Vec<bool>) -> u64 {
	let mut reader = Reader {
		slice: input.as_slice(),
		index: 0,
		max_length: input.len(),
	};

	let root = parse_packet(&mut reader);
	// println!("{:?}", root);

	sum_of_version(&root)
}

fn process(packet: &Packet) -> u64 {
	match &packet.value {
		TypeID::Literal(n) => *n,
		TypeID::Op(op, subpackets) => match *op {
			0 => subpackets.iter().map(&process).sum(),
			1 => subpackets.iter().map(&process).product(),
			2 => subpackets.iter().map(&process).min().unwrap(),
			3 => subpackets.iter().map(&process).max().unwrap(),
			5 => (process(&subpackets[0]) > process(&subpackets[1])) as u64,
			6 => (process(&subpackets[0]) < process(&subpackets[1])) as u64,
			7 => (process(&subpackets[0]) == process(&subpackets[1])) as u64,

			_ => unreachable!(),
		},
	}
}

fn part2(input: Vec<bool>) -> u64 {
	let mut reader = Reader {
		slice: input.as_slice(),
		index: 0,
		max_length: input.len(),
	};

	let root = parse_packet(&mut reader);
	// println!("{:?}", root);

	process(&root)
}
