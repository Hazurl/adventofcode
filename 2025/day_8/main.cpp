#include "../common.hpp"
#include <tinge/tinge.hpp>

using JunctionBox = std::tuple<long, long, long>;
using BoxID = std::size_t;
using CircuitID = std::size_t;

std::vector<JunctionBox> parse(std::vector<std::string> const &lines) {
  std::vector<JunctionBox> boxes;
  for (auto const &line : lines) {
    std::stringstream stream{line};
    auto const values = split(stream, ',');
    panic_if(values.size() != 3, "oops, parsed ", values.size(), " values on ",
             line);
    boxes.emplace_back(std::stol(values[0]), std::stol(values[1]),
                       std::stol(values[2]));
  }
  return boxes;
}

double compute_distance(JunctionBox const &lhs, JunctionBox const &rhs) {
  auto const dx = std::get<0>(rhs) - std::get<0>(lhs);
  auto const dy = std::get<1>(rhs) - std::get<1>(lhs);
  auto const dz = std::get<2>(rhs) - std::get<2>(lhs);
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

struct Link {
  std::size_t box_1{};
  std::size_t box_2{};
  double distance{};
};

bool perform_link(std::vector<CircuitID> &box_to_circuit,
                  std::vector<std::vector<BoxID>> &circuits, Link const &link) {
  auto new_circuit = box_to_circuit.at(link.box_1);
  auto const previous_circuit = box_to_circuit.at(link.box_2);

  if (new_circuit == previous_circuit) {
    return false;
  }

  panic_if(circuits.at(previous_circuit).empty() ||
               circuits.at(new_circuit).empty(),
           "unsync");

  for (auto const box : circuits[previous_circuit]) {
    box_to_circuit[box] = new_circuit;
  }

  circuits[new_circuit].append_range(circuits[previous_circuit]);
  circuits[previous_circuit].clear();
  return true;
}

long part_1(std::vector<JunctionBox> const &boxes) {
  std::vector<CircuitID> box_to_circuit(boxes.size());
  std::vector<std::vector<BoxID>> circuits(boxes.size());

  std::vector<Link> candidate_links;
  candidate_links.reserve(boxes.size() * (boxes.size() - 1) / 2);

  for (std::size_t i = 0; i < boxes.size(); ++i) {
    box_to_circuit.at(i) = i;
    circuits.at(i) = {i};
    for (std::size_t j = i + 1; j < boxes.size(); ++j) {
      candidate_links.emplace_back(
          Link{i, j, compute_distance(boxes.at(i), boxes.at(j))});
    }
  }

  auto const link_count = std::min<std::size_t>(1000, candidate_links.size());

  std::partial_sort(
      std::begin(candidate_links), std::begin(candidate_links) + link_count,
      std::end(candidate_links), [](auto const &lhs, auto const &rhs) {
        return lhs.distance < rhs.distance;
      });

  for (std::size_t i = 0; i < link_count; ++i) {
    auto const &candidate_link = candidate_links.at(i);
    perform_link(box_to_circuit, circuits, candidate_link);
  }

  auto const circuit_count = 3;

  std::partial_sort(std::begin(circuits), std::begin(circuits) + circuit_count,
                    std::end(circuits), [](auto const &lhs, auto const &rhs) {
                      return lhs.size() > rhs.size();
                    });

  return std::accumulate(
      std::begin(circuits), std::begin(circuits) + circuit_count, 1l,
      [](auto acc, auto const &circuit) { return acc * circuit.size(); });
}

long part_2(std::vector<JunctionBox> const &boxes) {
  std::vector<CircuitID> box_to_circuit(boxes.size());
  std::vector<std::vector<BoxID>> circuits(boxes.size());

  std::vector<Link> candidate_links;

  for (std::size_t i = 0; i < boxes.size(); ++i) {
    box_to_circuit[i] = i;
    circuits[i] = {i};
    for (std::size_t j = i + 1; j < boxes.size(); ++j) {
      candidate_links.emplace_back(
          Link{i, j, compute_distance(boxes[i], boxes[j])});
    }
  }

  std::sort(std::begin(candidate_links), std::end(candidate_links),
            [](auto const &lhs, auto const &rhs) {
              return lhs.distance < rhs.distance;
            });

  for (auto const candidate_link : candidate_links) {
    if (perform_link(box_to_circuit, circuits, candidate_link)) {
      if (circuits[box_to_circuit[candidate_link.box_1]].size() ==
          boxes.size()) {
        return std::get<0>(boxes[candidate_link.box_1]) *
               std::get<0>(boxes[candidate_link.box_2]);
      }
    }
  }

  panic("oops");
}

int main() {
  PROFILE_FUNCTION();

  auto const boxes = []() {
    PROFILE_SCOPE("Reading file");
    auto file = open_file("input.txt");
    return parse(lines_of(file));
  }();

  {
    PROFILE_SCOPE("Caching");
    for (int _ = 0; _ < 3; ++_) {
      {
        PROFILE_SCOPE("Part 1");
        part_1(boxes);
      }

      {
        PROFILE_SCOPE("Part 2");
        part_2(boxes);
      }
    }
  }

  {
    PROFILE_SCOPE("Result");
    {
      PROFILE_SCOPE("Part 1");
      tinge::println("[Part 1]: ", part_1(boxes));
    }

    {
      PROFILE_SCOPE("Part 2");
      tinge::println("[Part 2]: ", part_2(boxes));
    }
  }
}