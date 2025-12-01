#include "../common.hpp"
#include <tinge/tinge.hpp>

namespace p1 {
enum class Direction { Right, Left };

std::tuple<Direction, long> parse_line(std::string const &line) {
  if (line.size() < 2) {
    tinge::errorln("Line cannot be parsed: ", line);
    throw std::runtime_error("Line cannot be parsed");
  }
  std::stringstream sstream{line};
  const auto [direction, amount] = extract<char, long>(sstream);
  return {direction == 'L' ? Direction::Left : Direction::Right, amount};
}
} // namespace p1

long part_1(std::vector<std::string> const &lines) {
  long dial = 50;
  long dial_at_zero = 0;

  for (auto const &line : lines) {
    const auto [direction, amount] = p1::parse_line(line);
    if (direction == p1::Direction::Right) {
      dial = (dial + amount) % 100;
      //   tinge::println(line, " -> Right ", amount, " => ", dial);
    } else {
      dial = (dial - amount % 100 + 100) % 100;
      //   tinge::println(line, " -> Left ", amount, " => ", dial);
    }
    if (dial == 0) {
      ++dial_at_zero;
    }
  }
  return dial_at_zero;
}

namespace p2 {}

long part_2(std::vector<std::string> const &lines) {
  long dial = 50;
  long dial_at_zero = 0;

  for (auto const &line : lines) {
    const auto [direction, amount] = p1::parse_line(line);
    if (direction == p1::Direction::Right) {
      dial += amount;
      // tinge::println(line, " -> Right ", amount, " => ", dial);
      while (dial >= 100) {
        ++dial_at_zero;
        dial -= 100;
        // tinge::println("  # ", dial);
      }
    } else {
      const auto was_zero = dial == 0;
      dial -= amount;

      if (amount > 0 && was_zero) {
        // Don't count a `0` when starting at `0`
        dial += 100;
      }

      // tinge::println(line, " -> Left ", amount, " => ", dial);
      while (dial < 0) {
        ++dial_at_zero;
        dial += 100;
        // tinge::println("  # ", dial);
      }
      if (dial == 0) {
        ++dial_at_zero;
        // tinge::println("  # ", dial);
      }
    }
  }
  return dial_at_zero;
}

int main() {
  PROFILE_FUNCTION();

  auto lines = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return lines_of(file);
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(lines);
      }

      {
        PROFILE_SCOPE("Part 2");
        part_2(lines);
      }
    }
  }

  {
    PROFILE_SCOPE("Result");
    {
      PROFILE_SCOPE("Part 1");
      tinge::println("[Part 1]: ", part_1(lines));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(lines));
    }
  }
}