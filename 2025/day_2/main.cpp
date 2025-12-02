#include "../common.hpp"
#include <tinge/tinge.hpp>

struct Range {
  long from;
  long to;
};

Range parse_range(std::string const &raw_range) {
  auto const ids = split(raw_range, '-');
  if (ids.size() != 2) {
    panic("Bad range `", raw_range, "` (got ", ids.size(), " IDs)");
  }
  return Range{
      std::stol(ids[0]),
      std::stol(ids[1]),
  };
}

namespace p1 {
long count_digits(long value) {
  long count = 0;
  for (; value > 0; ++count) {
    value /= 10;
  }
  return count;
}

long pow10(long v) { return static_cast<long>(std::pow(10, v)); }

long get_minimum_to_repeat(Range const &range) {
  // Find the minimum numbers with an even amount of digits that is in the range
  long from = -1;
  if (count_digits(range.from) % 2 != 0) {
    // If range.from is uneven, but range.to is a superior power of 10
    // We can start our search on the next power of 10
    from = pow10(count_digits(range.from));
  } else {
    from = range.from;
  }
  if (from > range.to) {
    // tinge::noticeln(" asymmetric range");
    return -1;
  }

  auto const digits = count_digits(from);
  auto const split = pow10(digits / 2l);
  auto to_repeat = from / split;

  // tinge::noticeln("from=", from, " to_repeat=", to_repeat);
  return to_repeat;
}

long sum_invalid_ids(Range const &range) {
  // tinge::noticeln("For range <", range.from, " - ", range.to, ">");
  long count = 0;

  auto to_repeat = get_minimum_to_repeat(range);
  if (to_repeat < 0) {
    return 0;
  }

  for (;; ++to_repeat) {
    auto const factor = pow10(count_digits(to_repeat));
    auto const invalid_id = to_repeat + to_repeat * factor;

    if (invalid_id > range.to) {
      break;
    }

    if (range.from <= invalid_id) {
      // tinge::noticeln(" --> ", invalid_id);
      count += invalid_id;
    } else {
      // tinge::noticeln(" -/> ", invalid_id);
    }
  }

  return count;
}
} // namespace p1

long part_1(std::vector<Range> const &ranges) {
  long count = 0;
  for (auto const range : ranges) {
    count += p1::sum_invalid_ids(range);
  }
  return count;
}

namespace p2 {
long sum_invalid_ids_of_sub_range(Range const &range) {
  if (p1::count_digits(range.from) != p1::count_digits(range.to)) {
    panic("This algo assume the range doesn't cover multiples power of 10s");
  }

  long count = 0;

  // There's probably a better way to not count twice invalid IDs with multiple
  // repeated numbers. Example: `2222` has 2 repeats: `'22' * 2` and `'2' * 4`
  std::unordered_set<long> seen;

  auto const total_digits = p1::count_digits(range.from);

  // Find all factors of `total_digits` that divides that length in at least 2
  // groups
  for (auto repeated_digit_count = 1; total_digits / repeated_digit_count >= 2;
       ++repeated_digit_count) {
    if (total_digits % repeated_digit_count != 0) {
      continue;
    }
    auto const repeats = total_digits / repeated_digit_count;

    // Get the left most `repeated_digit_count` digits of `range.from`.
    // Example: repeated_digit_count=2, from=123456 ->  to_repeat=12
    auto const factor = p1::pow10(repeated_digit_count);
    auto to_repeat = range.from;
    for (; to_repeat >= factor; to_repeat /= factor)
      ;

    // tinge::noticeln("  repeated_digit_count=", repeated_digit_count, "
    // repeats=", repeats,
    //                 " factor=", factor, " to_repeat=", to_repeat);

    // Construct all possibles invalid ids from `to_repeat`
    // Example: to_repeat=12 -> invalid_ids=1212, 1313, 1414, ...
    // Until range.to
    for (; to_repeat < factor; ++to_repeat) {
      auto invalid_id = to_repeat;
      for (auto r = 1; r < repeats; ++r) {
        invalid_id = invalid_id * factor + to_repeat;
      }

      if (invalid_id > range.to) {
        // tinge::noticeln("  -/> ", invalid_id);
        break;
      }

      if (seen.contains(invalid_id)) {
        // tinge::noticeln("  -/> ", invalid_id);
        continue;
      }

      // We-ve got to check that again because to_repeat may have truncated
      // range.from. Example: from=1234 -> to_repeat=12 -> invalid_ids=1212
      if (range.from <= invalid_id) {
        // tinge::noticeln("  --> ", invalid_id);
        count += invalid_id;
        seen.emplace(invalid_id);
      } else {
        // tinge::noticeln("  -/> ", invalid_id);
      }
    }
  }

  return count;
}

long sum_invalid_ids(Range const &range) {
  // tinge::noticeln("For range <", range.from, " - ", range.to, ">");
  long count = 0;

  auto const max_digits = p1::count_digits(range.to);
  // Split the range in multiple sub-ranges, each with the same amount of digits
  for (auto digits = p1::count_digits(range.from); digits <= max_digits;
       ++digits) {
    auto const from = p1::pow10(digits - 1);
    auto const to = p1::pow10(digits) - 1;
    auto const sub_range =
        Range{std::max(range.from, from), std::min(range.to, to)};
    // tinge::noticeln(" Sub range <", sub_range.from, " - ", sub_range.to,
    // ">");
    count += p2::sum_invalid_ids_of_sub_range(sub_range);
  }
  return count;
}

} // namespace p2

long part_2(std::vector<Range> const &ranges) {
  long count = 0;
  for (auto const range : ranges) {
    count += p2::sum_invalid_ids(range);
  }
  return count;
}

int main() {
  PROFILE_FUNCTION();

  auto ranges = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    auto const raw_ranges = split(file, ',');
    std::vector<Range> ranges;
    ranges.reserve(raw_ranges.size());
    for (auto const &raw_range : raw_ranges) {
      ranges.emplace_back(parse_range(raw_range));
    }
    return ranges;
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(ranges);
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
      tinge::println("[Part 1]: ", part_1(ranges));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(ranges));
    }
  }
}