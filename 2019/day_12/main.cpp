#include "../common.hpp"

namespace p1 {

struct Vec {
    long x, y, z;

    bool operator == (Vec const& v) const {
        return v.x == x && v.y == y && v.z == z;
    }
};

struct Dim {
    long p, v;
    bool operator == (Dim const& d) const {
        return d.p == p && d.v == v;
    }
};

long energy(Vec const& v) {
    return std::abs(v.x) + std::abs(v.y) + std::abs(v.z);
}

long signum(long a) {
    return a < 0 ? -1 : a > 0 ? 1 : 0;
}

std::vector<std::pair<p1::Vec, p1::Vec>> step_once(std::vector<std::pair<p1::Vec, p1::Vec>> const& planets) {
    std::vector<std::pair<p1::Vec, p1::Vec>> new_planets = planets;

    // Gravity
    for(std::size_t i = 0; i < planets.size(); ++i) {
        for(std::size_t j = i+1; j < planets.size(); ++j) {
            auto& p0 = planets[i];
            auto& p1 = planets[j];

            auto pos0 = p0.first;
            auto pos1 = p1.first;

            new_planets[i].second.x += signum(pos1.x - pos0.x);
            new_planets[i].second.y += signum(pos1.y - pos0.y);
            new_planets[i].second.z += signum(pos1.z - pos0.z);

            new_planets[j].second.x += signum(pos0.x - pos1.x);
            new_planets[j].second.y += signum(pos0.y - pos1.y);
            new_planets[j].second.z += signum(pos0.z - pos1.z);
        }
    }

    for(auto& p : new_planets) {
        p.first.x += p.second.x;
        p.first.y += p.second.y;
        p.first.z += p.second.z;
    }

    return new_planets;
}

std::vector<Dim> step_once(std::vector<Dim> const& d) {
    std::vector<Dim> new_d = d;

    for(std::size_t i = 0; i < d.size(); ++i) {
        for(std::size_t j = i+1; j < d.size(); ++j) {
            auto& p0 = d[i];
            auto& p1 = d[j];

            auto pos0 = p0.p;
            auto pos1 = p1.p;

            new_d[i].v += signum(pos1 - pos0);
            new_d[j].v += signum(pos0 - pos1);
        }
    }

    for(auto& p : new_d) {
        p.p += p.v;
    }

    return new_d;
}

long find_cycle(std::vector<Dim> d) {
    long i = 0;
    auto zero = d;

    do {
        d = step_once(d);
        ++i;
    } while(zero != d);

    return i;
}

void print(std::vector<std::pair<Vec, Vec>> const& planets) {
    for(auto const& p : planets) {
        std::cout << "pos=<x=" << p.first.x << ", y=" << p.first.y << ", z=" << p.first.z << "> ~" << energy(p.first) << ", vel=<x=" << p.second.x << ", y=" << p.second.y << ", z=" << p.second.z << "> _" << energy(p.second) << "\n";
    }
} 

}

long part_1(std::vector<std::pair<p1::Vec, p1::Vec>> planets) {
    //std::cout << "After " << 0 << " steps:\n";
    //p1::print(planets);

    for(int i = 0; i < 1000; ++i) {
        planets = p1::step_once(planets);

        //std::cout << "After " << i + 1 << " steps:\n";
        //p1::print(planets);
    }

    return std::accumulate(std::begin(planets), std::end(planets), 0L, [] (long acc, auto const& p) { return acc + p1::energy(p.first) * p1::energy(p.second); });
}

namespace p2 {

}

long part_2(std::vector<std::pair<p1::Vec, p1::Vec>> planets) {
    auto first_state = planets;
    std::cout << "After " << 0 << " steps:\n";
    p1::print(planets);

    for(long i = 0;; ++i) {
        planets = p1::step_once(planets);

        if ((i + 1) % 100'000'000 == 0) {
            std::cout << (i + 1) / 100'000'000 << '\n';
        }

        auto z = std::count_if(std::begin(planets), std::end(planets), [] (auto const& p) {
            return p.second == p1::Vec{ 0, 0, 0 };
        });

        //if (std::all_of(std::begin(planets), std::end(planets), [] (auto const& p) {
        //    return p.second == p1::Vec{ 0, 0, 0 };
        //})) {
        //    return i*2 + 2;
        //}

        if (z > 1) {
            std::cout << "After " << i + 1 << " steps: ";
            std::cout << "#" << z;
            std::cout << '\n';
            p1::print(planets);
        }
    }

    return -1;
}

long part_2_dim(std::vector<std::pair<p1::Vec, p1::Vec>> const& planets) {
    std::vector<p1::Dim> dx; dx.reserve(planets.size());
    std::vector<p1::Dim> dy; dy.reserve(planets.size());
    std::vector<p1::Dim> dz; dz.reserve(planets.size());

    for(auto const& p : planets) {
        auto[pos, vel] = p;
        dx.push_back({ pos.x, vel.x });
        dy.push_back({ pos.y, vel.y });
        dz.push_back({ pos.z, vel.z });
    }

    long ix = p1::find_cycle(dx);
    long iy = p1::find_cycle(dy);
    long iz = p1::find_cycle(dz);
    long g = std::gcd(std::gcd(ix, iy), iz);


    std::cout << ix << ", " << iy << ", " << iz << ", " << ix * iy * iz << '\n';
    std::cout << ix / g << ", " << iy / g << ", " << iz / g << ", " << (ix / g) * (iy / g) * (iz / g) << '\n';

    return (ix / g) * (iy / g) * (iz / g);
}

int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        std::vector<std::pair<p1::Vec, p1::Vec>> planets;
        auto lines = lines_of(file);
        planets.reserve(lines.size());
        for(auto const& l : lines) {
            std::stringstream ss(l);
            p1::Vec pos;
            ss >> pos.x >> pos.y >> pos.z;
            planets.emplace_back(pos, p1::Vec{ 0, 0, 0 });
        }
        return planets;
    }();

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
            tinge::println("[Part 2]: ", part_2_dim(values));
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