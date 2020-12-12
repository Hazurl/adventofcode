#include "../common.hpp"

long part_1(std::vector<std::string> const& lines) {
    std::pair<long, long> north_offset{ 0, 1 };
    std::pair<long, long> east_offset{ 1, 0 };
    std::pair<long, long> south_offset{ 0, -1 };
    std::pair<long, long> west_offset{ -1, 0 };

    std::array directions_offset{
        north_offset, east_offset, south_offset, west_offset
    };

    std::size_t direction = 1;
    long pos_x = 0;
    long pos_y = 0;

    auto move = [&] (auto const& offset, long amount) {
        pos_x += amount * offset.first;
        pos_y += amount * offset.second;
    };

    for(auto const& line : lines) {
        auto c_str = line.c_str();
        auto num_str = c_str + 1;
        auto amount = std::stol(num_str);

        switch(line[0]) {
            case 'N': move(north_offset, amount); break;
            case 'E': move(east_offset, amount); break;
            case 'S': move(south_offset, amount); break;
            case 'W': move(west_offset, amount); break;
            case 'L': direction += 3 * amount / 90; break;
            case 'R': direction += amount / 90; break;
            case 'F': move(directions_offset[direction % directions_offset.size()], amount); break;
        }
    }

    return std::abs(pos_x) + std::abs(pos_y);
}

namespace p2 {

}

long part_2(std::vector<std::string> const& lines) {
    std::pair<long, long> north_offset{ 0, 1 };
    std::pair<long, long> east_offset{ 1, 0 };
    std::pair<long, long> south_offset{ 0, -1 };
    std::pair<long, long> west_offset{ -1, 0 };

    long pos_x = 0;
    long pos_y = 0;

    long waypoint_x = 10;
    long waypoint_y = 1;

    auto move_ship = [&] (auto const& offset, long amount) {
        pos_x += amount * offset.first;
        pos_y += amount * offset.second;
    };

    auto move_waypoint = [&] (auto const& offset, long amount) {
        waypoint_x += amount * offset.first;
        waypoint_y += amount * offset.second;
    };

    auto rotate_waypoint = [&] (long amount) {
        amount %= 4;
        while(amount--) {
            auto tmp = waypoint_x;
            waypoint_x = waypoint_y;
            waypoint_y = -tmp;
        }
    };

    for(auto const& line : lines) {
        auto c_str = line.c_str();
        auto num_str = c_str + 1;
        auto amount = std::stol(num_str);

        switch(line[0]) {
            case 'N': move_waypoint(north_offset, amount); break;
            case 'E': move_waypoint(east_offset, amount); break;
            case 'S': move_waypoint(south_offset, amount); break;
            case 'W': move_waypoint(west_offset, amount); break;
            case 'L': rotate_waypoint(3 * amount / 90); break;
            case 'R': rotate_waypoint(amount / 90); break;
            case 'F': move_ship(std::pair{ waypoint_x, waypoint_y }, amount); break;
        }
    }

    return std::abs(pos_x) + std::abs(pos_y);
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
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