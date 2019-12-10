#include "../common.hpp"

namespace p1 {

using Map = std::vector<std::vector<bool>>;

std::pair<Map, std::vector<std::pair<long, long>>> map_asteroids(std::vector<std::string> const& values) {
    Map map(values.size());
    std::vector<std::pair<long, long>> asteroids;

    long y = 0;
    auto it = std::begin(map);
    for(auto const& line : values) {

        auto& as = *it++;
        long x = 0;

        for(auto a : line) {
            bool is_asteroid = a == '#';
            as.push_back(is_asteroid);
            if (is_asteroid) {
                asteroids.push_back({ x, y });
            }

            ++x;
        }

        ++y;
    }

    return { map, asteroids };
}

long count_asteroids_in_los(std::pair<long, long> from, std::pair<long, long> to, Map const& map) {
    long count = 0;

    long dx = to.first - from.first;
    long dy = to.second - from.second;
    long gcd = std::gcd(dx, dy);
    long step_dx = dx / gcd;
    long step_dy = dy / gcd;

    long cx = from.first+ step_dx;
    long cy = from.second + step_dy;

    while(--gcd > 0) {

        if (map[cy][cx]) {
            ++count;
        }

        cx += step_dx;
        cy += step_dy;
    }

    return count;
}

long count_asteroids_in_sight(long x, long y, Map const& map, std::vector<std::pair<long, long>> const& asteroids) {
    long count = 0;

    for(auto const& as : asteroids) {
        if (as == std::pair{ x, y }) {
            continue;
        }

        if (count_asteroids_in_los({ x, y }, as, map) == 0) {
            ++count;
        }
    }

    return count;
}

std::pair<std::pair<long, long>, long> get_best_most_location(Map const& map, std::vector<std::pair<long, long>> const& asteroids) {
    long max_count = 0;

    std::pair<long, long> pos;

    for(auto const& as : asteroids) {
        auto count = p1::count_asteroids_in_sight(as.first, as.second, map, asteroids);
        if (count > max_count) {
            max_count = count;
            pos = as;
        }
    }

    return { pos, max_count };
}

}

long part_1(std::vector<std::string> const& values) {
    auto&&[map, asteroids] = p1::map_asteroids(values);
    return p1::get_best_most_location(map, asteroids).second;
}

namespace p2 {

double angle_of_vec(std::pair<long, long> const& p) {
    double x = p.first;
    double y = p.second;
    double d = std::sqrt(x*x+y*y);

    return 180. + std::atan2(-x/d, y/d) / 6.28318530718 * 360.0;
}

struct Asteroid {
    std::pair<long, long> pos;
    long asteroids_blocking;
    double angle;

    friend std::ostream& operator<<(std::ostream& os, Asteroid const& a) {
        return os << "[[" << a.pos.first << ", " << a.pos.second << "]] a=" << a.angle << " b=" << a.asteroids_blocking << '\n';
    }

    bool operator<(Asteroid const& o) const {
        if (asteroids_blocking == o.asteroids_blocking) {
            return angle < o.angle;
        }
        return asteroids_blocking < o.asteroids_blocking;
    }
};

long get_nth(std::vector<std::string> const& values, std::size_t i) {
    auto&&[map, asteroids] = p1::map_asteroids(values);

    auto pos = p1::get_best_most_location(map, asteroids).first;
    //std::pair<long, long> pos = {8,3};
    DBG(pos);

    std::vector<Asteroid> as;

    for(auto const& a : asteroids) {
        if (a == pos) continue;
        long dx = a.first - pos.first;
        long dy = a.second - pos.second;
        double angle = p2::angle_of_vec({ dx, dy });
        long blocking = p1::count_asteroids_in_los(pos, a, map);

        as.push_back({ a, blocking, angle });

        //DBG(dx, ", ", dy, ": ", gcd, " <= ", as.back());
    }
/*
    std::nth_element(std::begin(as), std::begin(as) + as.size(), std::end(as), 
        [] (auto lhs, auto rhs) {
            if (lhs.dist == rhs.dist) {
                return lhs.angle < rhs.angle;
            }
            return lhs.dist < rhs.dist;
        });*/

    std::sort(std::begin(as), std::end(as));

    DBG(as, "size: ", as.size());

    return as[i].pos.first * 100 + as[i].pos.second;
}

}

long part_2(std::vector<std::string> const& values) {
    return p2::get_nth(values, 199);
}

int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();

    DBG("angle_of_vec ", p2::angle_of_vec({0, 1}));
    DBG("angle_of_vec ", p2::angle_of_vec({1, 0}));
    DBG("angle_of_vec ", p2::angle_of_vec({0, -1}));
    DBG("angle_of_vec ", p2::angle_of_vec({-1, 0}));
    DBG("angle_of_vec ", p2::angle_of_vec({0, 2}));
    DBG("angle_of_vec ", p2::angle_of_vec({2, 0}));
    DBG("angle_of_vec ", p2::angle_of_vec({0, -2}));
    DBG("angle_of_vec ", p2::angle_of_vec({-2, 0}));

/*
    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(values);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(values);
            }
        }
    }*/

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values));
        }
    }
/*
    {
        PROFILE_PART(1);
        part_1(values);
    }

    {
        PROFILE_PART(2);
        part_2(values);
    }*/
}