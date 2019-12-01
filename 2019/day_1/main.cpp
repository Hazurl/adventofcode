#include "../common.hpp"

namespace p1 {

long find_fuel_required(long mass) {
    return mass / 3 - 2;
}

std::vector<long> convert_to_long(std::vector<std::string> const& lines) {
    PROFILE_FUNCTION();
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

void part_1(std::vector<std::string> const& lines) {
    auto values = p1::convert_to_long(lines);

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

void part_2(std::vector<std::string> const& lines) {
    auto values = p1::convert_to_long(lines);

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

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        return read_file("input.txt");
    }();

    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
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
        PROFILE_PART(1);
        part_1(lines);
    }

    {
        PROFILE_PART(2);
        part_2(lines);
    }
}