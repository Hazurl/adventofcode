#include "../common.hpp"

namespace p1 {

long find_fuel_required(long mass) {
    return mass / 3 - 2;
}

std::vector<long> read_values() {
    auto lines = read_file("input.txt");

    std::vector<long> values;
    values.reserve(lines.size());

    {
        PROFILE_SCOPE("transfrom string to values");
        std::transform(std::begin(lines), std::end(lines), std::back_inserter(values), 
            [] (auto const& line) {
                return std::stol(line);
            });
    }

    return values;
}

}

void part_1() {
    auto values = p1::read_values();

    long total_fuel = [&] {
        PROFILE_SCOPE("sum fuels");
        return std::accumulate(std::begin(values), std::end(values), 0L, 
            [] (long total_fuel, long mass) {
                return total_fuel + p1::find_fuel_required(mass);
            });
    }();

    tinge::println("Fuel required: ", total_fuel);
}

namespace p2 {

long find_fuel_required(long mass, long fuel = 0) {
    long fuel_for_mass = p1::find_fuel_required(mass);
    if (fuel_for_mass < 0) {
        return fuel;
    } else {
        return find_fuel_required(fuel_for_mass, fuel + fuel_for_mass);
    }
}
    
}

void part_2() {
    auto values = p1::read_values();

    long total_fuel = [&] {
        PROFILE_SCOPE("sum fuels");
        return std::accumulate(std::begin(values), std::end(values), 0L, 
            [] (long total_fuel, long mass) {
                return total_fuel + p2::find_fuel_required(mass);
            });
    }();

    tinge::println("Fuel required: ", total_fuel);
}

int main() {
    PROFILE_FUNCTION();

    {
        PROFILE_SCOPE("Part 1");
        part_1();
    }

    {
        PROFILE_SCOPE("Part 2");
        part_2();
    }
}