#include "../common.hpp"
#include <tinge/tinge.hpp>

struct Input {
  std::vector<std::unordered_set<long>> splits;
  long start;
};

Input parse(std::vector<std::string> const &lines) {
  Input input{};
  for (std::size_t i = 0; i < lines[0].size(); ++i) {
    if (lines[0][i] == 'S') {
      input.start = static_cast<long>(i);
    }
  }

  for (std::size_t l = 1; l < lines.size(); ++l) {
    std::unordered_set<long> splits;
    for (long i = 0; i < static_cast<long>(lines[l].size()); ++i) {
      if (lines[l][i] == '^') {
        splits.insert(i);
      }
    }
    if (!splits.empty()) {
      input.splits.emplace_back(std::move(splits));
    }
  }
  return input;
}

long part_1(Input const &input) {
  std::unordered_set<long> beams;
  std::unordered_set<long> next_beams;

  long split_count = 0;

  beams.insert(input.start);
  for (auto const &splits : input.splits) {
    for (auto const beam : beams) {
      if (splits.contains(beam)) {
        next_beams.insert(beam - 1);
        next_beams.insert(beam + 1);
        ++split_count;
      } else {
        next_beams.insert(beam);
      }
    }
    beams = std::move(next_beams);
    next_beams = {};
  }

  return split_count;
}

long part_2(Input const &input) {
  std::unordered_map<long, long> beams;
  std::unordered_map<long, long> next_beams;

  beams.emplace(input.start, 1);
  for (auto const &splits : input.splits) {
    for (auto const [beam, count] : beams) {
      if (splits.contains(beam)) {
        next_beams.insert_or_assign(beam - 1,
                                    count + try_get(next_beams, beam - 1, 0l));
        next_beams.insert_or_assign(beam + 1,
                                    count + try_get(next_beams, beam + 1, 0l));
      } else {
        next_beams.insert_or_assign(beam,
                                    count + try_get(next_beams, beam, 0l));
      }
    }
    beams = std::move(next_beams);
    next_beams = {};
  }

  return std::accumulate(
      std::begin(beams), std::end(beams), 0l,
      [](auto acc, auto const &pair) { return acc + pair.second; });
}

int main() {
  PROFILE_FUNCTION();

  auto const input = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return parse(lines_of(file));
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(input);
      }

      {
        PROFILE_SCOPE("Part 2");
        part_2(input);
      }
    }
  }

  {
    PROFILE_SCOPE("Result");
    {
      PROFILE_SCOPE("Part 1");
      tinge::println("[Part 1]: ", part_1(input));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(input));
    }
  }
}