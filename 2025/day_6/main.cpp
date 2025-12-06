#include "../common.hpp"
#include <tinge/tinge.hpp>

enum class Op { Add, Mul };

struct Problem {
  std::vector<long> values;
  Op op;

  long evaluate() const {
    if (op == Op::Add) {
      return std::accumulate(std::begin(values), std::end(values), 0l,
                             std::plus{});
    } else {
      return std::accumulate(std::begin(values), std::end(values), 1l,
                             std::multiplies{});
    }
  }
};
namespace p1 {
struct Problems {
  std::vector<std::vector<long>> values;
  std::vector<Op> ops;

  Problem get_problem(std::size_t i) const {
    Problem problem{{}, ops[i]};
    for (auto const &row : values) {
      problem.values.push_back(row[i]);
    }
    return problem;
  }
};

Problems parse(std::vector<std::string> const &lines) {
  Problems problems;

  for (auto const &line : lines) {
    std::stringstream stream{line};
    if (line[0] == '+' || line[0] == '*') {
      for (auto c : spaces_separated<char>(stream)) {
        problems.ops.push_back(c == '+' ? Op::Add : Op::Mul);
      }
    } else {
      problems.values.emplace_back(spaces_separated<long>(stream));
    }
  }

  auto const size = problems.ops.size();
  // tinge::successln("Found ", size, " problems");
  for (auto const &row : problems.values) {
    panic_if(row.size() != size, "Oops, found ", row.size());
  }

  return problems;
}
} // namespace p1

long part_1(std::vector<std::string> const &lines) {
  auto const problems = p1::parse(lines);
  long sum = 0;
  for (std::size_t i = 0; i < problems.ops.size(); ++i) {
    auto const pb = problems.get_problem(i);
    sum += pb.evaluate();
  }
  return sum;
}

namespace p2 {

std::vector<Problem> parse(std::vector<std::string> const &lines) {
  std::vector<Problem> problems;

  auto const len = lines[0].size();
  for (auto const &line : lines) {
    panic_if(line.size() != len, "Oops, found len=", line.size(), ", expected ",
             len);
  }

  Problem problem{};

  for (std::size_t i = 0; i < len; ++i) {
    long value = 0;
    for (auto const &line : lines) {
      auto c = line[i];
      if (c == ' ') {
        continue;
      } else if (c == '+') {
        problem.op = Op::Add;
      } else if (c == '*') {
        problem.op = Op::Mul;
      } else {
        panic_if(c < '0' || c > '9', "unknown charater '", c, "'");
        value = value * 10 + long{c - '0'};
      }
    }

    if (value == 0) {
      problems.emplace_back(std::move(problem));
      problem = Problem{};
      continue;
    }

    problem.values.push_back(value);
  }

  problems.emplace_back(std::move(problem));

  return problems;
}
} // namespace p2

long part_2(std::vector<std::string> const &lines) {
  auto const problems = p2::parse(lines);
  long sum = 0;
  for (auto const &pb : problems) {
    sum += pb.evaluate();
  }
  return sum;
}

int main() {
  PROFILE_FUNCTION();

  auto const lines = []() {
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