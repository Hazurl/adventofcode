#include "../common.hpp"
#include <tinge/tinge.hpp>

long count_neighbors_rolls(
    std::unordered_set<std::pair<std::size_t, std::size_t>> const &rolls,
    std::size_t const x, std::size_t const y) {

  long count = 0;

  for (long dy = -1; dy <= 1; ++dy) {
    auto const ey = static_cast<long>(y) + dy;
    for (long dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) {
        continue;
      }
      auto const ex = static_cast<long>(x) + dx;

      if (rolls.contains({ex, ey})) {
        ++count;
      }
    }
  }

  return count;
}

bool is_accessible_by_forklift(
    std::unordered_set<std::pair<std::size_t, std::size_t>> const &rolls,
    std::size_t const x, std::size_t const y) {
  auto const count = count_neighbors_rolls(rolls, x, y);
  return count < 4;
}

std::unordered_set<std::pair<std::size_t, std::size_t>>
transform_input(std::vector<std::string> const &inventory) {
  std::unordered_set<std::pair<std::size_t, std::size_t>> rolls;
  for (std::size_t y = 0; y < inventory.size(); ++y) {
    for (std::size_t x = 0; x < inventory[y].size(); ++x) {
      if (inventory[y][x] == '@') {
        rolls.emplace(x, y);
      }
    }
  }
  return rolls;
}

long part_1(std::vector<std::string> const &inventory) {
  auto rolls = transform_input(inventory);

  long count = 0;
  for (auto [x, y] : rolls) {
    if (is_accessible_by_forklift(rolls, x, y)) {
      ++count;
    }
  }
  return count;
}

long part_2(std::vector<std::string> const &inventory) {
  auto rolls = transform_input(inventory);

  long count = 0;
  bool some_roll_was_accessible = true;
  while (some_roll_was_accessible) {
    some_roll_was_accessible = false;
    for (auto [x, y] : rolls) {
      if (is_accessible_by_forklift(rolls, x, y)) {
        rolls.erase({x, y});
        ++count;
        some_roll_was_accessible = true;
        break;
      }
    }
  }
  return count;
}

int main() {
  PROFILE_FUNCTION();

  auto lines = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return lines_of(file);
  }();

  // {
  //   PROFILE_SCOPE("Caching");
  //   for (int _ = 0; _ < 3; ++_) {
  //     {
  //       PROFILE_SCOPE("Part 1");
  //       part_1(lines);
  //     }

  //     {
  //       PROFILE_SCOPE("Part 2");
  //       part_2(lines);
  //     }
  //   }
  // }

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