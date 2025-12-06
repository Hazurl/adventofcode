#include "../common.hpp"
#include <tinge/tinge.hpp>

using Range = std::pair<long, long>;

std::tuple<std::vector<Range>, std::vector<long>>
parse(std::vector<std::string> const &lines) {
  std::vector<Range> ranges;
  std::vector<long> ids;
  bool parsing_ids = false;

  for (auto const &line : lines) {
    if (line.size() == 0) {
      parsing_ids = true;
      continue;
    }

    std::stringstream stream{line};
    if (parsing_ids) {
      auto const [id] = extract<long>(stream);
      ids.emplace_back(id);
    } else {
      auto const [left_id, _, right_id] = extract<long, char, long>(stream);
      ranges.emplace_back(left_id, right_id);
    }
  }
  return {ranges, ids};
}

long part_1(std::vector<Range> ranges, std::vector<long> ids) {
  long count = 0;
  for (auto const id : ids) {
    for (auto const [left, right] : ranges) {
      if (left <= id && id <= right) {
        ++count;
        break;
      }
    }
  }

  return count;
}

long part_2(std::vector<Range> ranges) {
  std::sort(
      std::begin(ranges), std::end(ranges),
      [](auto const lhs, auto const rhs) { return lhs.first < rhs.first; });

  long count = 0;
  auto min_id = ranges[0].first - 1;
  for (auto const [left, right] : ranges) {
    min_id = std::max(min_id, left);
    if (right >= min_id) {
      count += (right - min_id) + 1;
      min_id = right + 1;
    }
  }

  return count;
}

int main() {
  PROFILE_FUNCTION();

  auto const [ranges, ids] = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return parse(lines_of(file, false));
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(ranges, ids);
      }

      {
        PROFILE_SCOPE("Part 2");
        part_2(ranges);
      }
    }
  }

  {
    PROFILE_SCOPE("Result");
    {
      PROFILE_SCOPE("Part 1");
      tinge::println("[Part 1]: ", part_1(ranges, ids));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(ranges));
    }
  }
}