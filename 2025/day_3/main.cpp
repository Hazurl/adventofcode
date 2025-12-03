#include "../common.hpp"
#include <tinge/tinge.hpp>

using Battery = long;
using Joltage = long;
using Bank = std::vector<Battery>;

Bank parse_bank(std::string const &line) {
  Bank bank;
  bank.reserve(line.size());
  for (auto c : line) {
    auto battery = static_cast<Battery>(c) - static_cast<Battery>('0');
    if (battery < 0 || battery > 9)
      panic("Bad battery parsing: ", battery, " in line ", line);
    bank.emplace_back(battery);
  }
  if (bank.size() < 2) {
    panic("Bank is too small in line ", line);
  }
  return bank;
}

std::vector<Bank> parse_banks(std::vector<std::string> const &lines) {
  std::vector<Bank> banks;
  banks.reserve(lines.size());
  for (auto const &line : lines) {
    banks.emplace_back(parse_bank(line));
  }
  return banks;
}

namespace p1 {
Joltage find_biggest_joltage(Bank const &bank) {
  Battery biggest_battery_on_the_right = bank[bank.size() - 1];
  long index = bank.size() - 2;
  Joltage max_joltage = biggest_battery_on_the_right + bank[index] * 10;
  for (; index >= 0; --index) {
    max_joltage =
        std::max(max_joltage, biggest_battery_on_the_right + bank[index] * 10);
    // tinge::noticeln("index=", index, " rbatt=", bank[index],
    //                 " lbatt=", biggest_battery_on_the_right,
    //                 " max_joltage=", max_joltage);
    biggest_battery_on_the_right =
        std::max(biggest_battery_on_the_right, bank[index]);
  }
  // tinge::warnln("Got ", max_joltage);
  return max_joltage;
}

} // namespace p1

long part_1(std::vector<Bank> const &banks) {
  long sum = 0;
  for (auto const &bank : banks) {
    auto const joltage = p1::find_biggest_joltage(bank);
    sum += joltage;
  }
  return sum;
}

namespace p2 {

using CacheKey = std::pair<std::size_t, std::size_t>;
using Cache = std::unordered_map<CacheKey, Joltage>;

Joltage find_biggest_joltage_rec(Bank const &bank, std::size_t after_index,
                                 std::size_t battery_count, Cache &cache) {
  if (battery_count == 0 || after_index + battery_count > bank.size()) {
    return 0;
  }

  auto const key = CacheKey{after_index, battery_count};
  if (cache.contains(key)) {
    return cache[key];
  }

  Joltage max_joltage = std::max(
      bank[after_index] * static_cast<long>(std::pow(10l, battery_count - 1l)) +
          find_biggest_joltage_rec(bank, after_index + 1, battery_count - 1,
                                   cache),
      find_biggest_joltage_rec(bank, after_index + 1, battery_count, cache));

  // tinge::noticeln("key=<", after_index, ", ", battery_count, "> -> ",
  //                 max_joltage);
  return cache.emplace(key, max_joltage).first->second;
}

Joltage find_biggest_joltage(Bank const &bank) {
  Cache cache;
  return find_biggest_joltage_rec(bank, 0, 12, cache);
}

} // namespace p2

long part_2(std::vector<Bank> const &banks) {
  long sum = 0;
  for (auto const &bank : banks) {
    auto const joltage = p2::find_biggest_joltage(bank);
    // tinge::warnln("Got ", joltage);
    sum += joltage;
  }
  return sum;
}

int main() {
  PROFILE_FUNCTION();

  auto banks = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return parse_banks(lines_of(file));
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(banks);
      }

      {
        PROFILE_SCOPE("Part 2");
        part_2(banks);
      }
    }
  }

  {
    PROFILE_SCOPE("Result");
    {
      PROFILE_SCOPE("Part 1");
      tinge::println("[Part 1]: ", part_1(banks));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(banks));
    }
  }
}