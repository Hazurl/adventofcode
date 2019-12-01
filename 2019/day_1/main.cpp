#include "../common.hpp"

namespace p1 {

long find_fuel_required(long mass) {
    return mass / 3 - 2;
}

}

long part_1(std::vector<std::string> const& lines) {
    long total_fuel = [&] {
        PROFILE_SCOPE("sum fuels");
        return std::accumulate(std::begin(lines), std::end(lines), 0L, 
            [] (long total_fuel, std::string const& mass) {
                return total_fuel + p1::find_fuel_required(std::stol(mass));
            });
    }();

    return total_fuel;
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

long find_fuel_required_no_tail(long mass) {
    long fuel_for_mass = p1::find_fuel_required(mass);
    if (fuel_for_mass < 0) {
        return 0;
    } else {
        return find_fuel_required_no_tail(fuel_for_mass) + fuel_for_mass;
    }
}

}

long part_2(std::vector<std::string> const& lines) {
    long total_fuel = [&] {
        PROFILE_SCOPE("sum fuels");
        return std::accumulate(std::begin(lines), std::end(lines), 0L, 
            [] (long total_fuel, std::string const& mass) {
                return total_fuel + p2::find_fuel_required(std::stol(mass));
            });
    }();

    return total_fuel;
}

long part_2_no_tail(std::vector<std::string> const& lines) {
    long total_fuel = [&] {
        PROFILE_SCOPE("sum fuels");
        return std::accumulate(std::begin(lines), std::end(lines), 0L, 
            [] (long total_fuel, std::string const& mass) {
                return total_fuel + p2::find_fuel_required_no_tail(std::stol(mass));
            });
    }();

    return total_fuel;
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

            {
                PROFILE_SCOPE("Part 2 no tail");
                part_2(lines);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1] Fuel required: ", part_1(lines));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2] Fuel required: ", part_2(lines));
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

    {
        PROFILE_PART(2_no_tail);
        part_2_no_tail(lines);
    }
}