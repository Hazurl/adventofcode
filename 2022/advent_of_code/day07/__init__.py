from dataclasses import dataclass
from typing import Generator, Optional, Union


def main() -> None:
    with open("./advent_of_code/day07/input.txt") as f:
        raw_input = f.read()

    print(f"Part 1: {part1(raw_input)}")
    print(f"Part 2: {part2(raw_input)}")


@dataclass
class File:
    name: str
    size: int


@dataclass
class Directory:
    name: str
    childrens: list[Union["Directory", File]]
    size: Optional[int] = None


def get_size(dir_or_file: Directory | File) -> int:
    if isinstance(dir_or_file, File):
        return dir_or_file.size
    if dir_or_file.size is None:
        dir_or_file.size = sum(get_size(child) for child in dir_or_file.childrens)
    return dir_or_file.size


@dataclass
class CdCommand:
    target: str

    def is_root(self):
        return self.target == "/"

    def is_up(self):
        return self.target == ".."


@dataclass
class LsCommand:
    childrens: list[Directory | File]


def parse_commands(lines: list[str]) -> list[CdCommand | LsCommand]:
    commands: list[CdCommand | LsCommand] = []
    for line in lines:
        if line.startswith("$"):
            line = line[2:]
            if line.startswith("cd"):
                line = line[3:]
                commands.append(CdCommand(line))
            elif line.startswith("ls"):
                commands.append(LsCommand([]))
        elif line.startswith("dir"):
            ls_command = commands[-1]
            assert isinstance(ls_command, LsCommand)
            ls_command.childrens.append(Directory(line[4:], []))
        else:
            size, name = line.split()
            ls_command = commands[-1]
            assert isinstance(ls_command, LsCommand)
            ls_command.childrens.append(File(name, int(size)))
    return commands


def construct_tree(commands: list[CdCommand | LsCommand]) -> Directory:
    root = Directory("/", [])
    currents = [root]
    for command in commands:
        if isinstance(command, CdCommand):
            if command.is_root():
                currents = [root]
            elif command.is_up():
                currents.pop()
            else:
                target_dir = next(child for child in currents[-1].childrens if child.name == command.target)
                if not isinstance(target_dir, Directory):
                    raise ValueError(f"Cannot cd into {command.target}")
                currents.append(target_dir)
        elif isinstance(command, LsCommand):
            currents[-1].childrens = command.childrens
    return root


def directories(root: Directory) -> Generator[Directory, None, None]:
    yield root
    for child in root.childrens:
        if isinstance(child, Directory):
            yield from directories(child)


def part1(raw_input: str) -> int:
    lines = raw_input.strip().splitlines()
    commands = parse_commands(lines)
    root = construct_tree(commands)

    return sum(get_size(directory) for directory in directories(root) if get_size(directory) <= 100000)


def part2(raw_input: str) -> int:
    lines = raw_input.strip().splitlines()
    commands = parse_commands(lines)
    root = construct_tree(commands)

    total_available_size = 70000000
    size_needed = 30000000
    root_size = get_size(root)
    threshold = size_needed - (total_available_size - root_size)
    return min(get_size(directory) for directory in directories(root) if get_size(directory) >= threshold)


if __name__ == "__main__":
    main()
