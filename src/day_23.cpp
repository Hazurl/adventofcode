#include <iostream>
#include "../formatted_inputs/day_23.inc"
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <tuple>
#include <unordered_map>
#include <cmath>


struct Vec3 {
    int x, y, z;
};

std::ostream& operator << (std::ostream& os, Vec3 const& v) {
    return os << v.x << ", " << v.y << ", " << v.z;
}

bool operator == (Vec3 const& l, Vec3 const& r) {
    return l.x == r.x && l.y == r.y && l.z == r.z;
}

Vec3 operator - (Vec3 const& l, Vec3 const& r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z
    };
}

long mag(Vec3 const& v) {
    return std::abs(static_cast<long>(v.x)) + std::abs(static_cast<long>(v.y)) + std::abs(static_cast<long>(v.z));
}

struct Nanobot {
    Vec3 pos;
    int r;
};

Nanobot parse(std::string_view const& s) {
    std::stringstream is(std::string{s});

    Nanobot n;

    assert(is.get() == 'p');
    assert(is.get() == 'o');
    assert(is.get() == 's');
    assert(is.get() == '=');
    assert(is.get() == '<');
    is >> n.pos.x;
    assert(is.get() == ',');
    is >> n.pos.y;
    assert(is.get() == ',');
    is >> n.pos.z;
    assert(is.get() == '>');
    assert(is.get() == ',');
    assert(is.get() == ' ');
    assert(is.get() == 'r');
    assert(is.get() == '=');
    is >> n.r;

    return n;
}

std::vector<Nanobot> parse_nanobots() {
    std::vector<Nanobot> ns;
    std::transform(std::begin(input), std::end(input), std::back_inserter(ns), parse);

    return ns;
}

struct Hasher {
    std::size_t operator() (Vec3 const& p) const {
        return p.x ^ p.y ^ p.z;
    }
};

using Depth = int;

struct Circle {
    int x, y;
    int r;
};

using Circles = std::unordered_map<Depth, Circle>;

using Slice = std::vector<Circle>;

using World = std::unordered_map<Depth, Slice>;

Circles nanobot_shape(Nanobot const& n) {
    Circles cs;
    for(int d{ -n.r }; d < n.r; ++d) {
        cs.emplace(n.pos.z + d, Circle{ n.pos.x, n.pos.y, n.r });
    }

    return cs;
}

int main() {
    auto ns = parse_nanobots();

    auto max_it = std::max_element(std::begin(ns), std::end(ns), [] (auto const& l, auto const& r) {
        return l.r < r.r;
    });

    std::cout << "Max: " << max_it->r << '\n';

    auto total = std::count_if(std::begin(ns), std::end(ns), [max_it] (auto const& n) {
        auto dist = mag(n.pos - max_it->pos);
        return dist <= max_it->r;
    });

    std::cout << "In range: " << total << '\n';

    std::unordered_set<Vec3, Hasher> possible_positions;
/*
    std::size_t i{ 0 };
    for(auto const& n : ns) {
        /*
        std::cout << "#" << i << '\n';
        ++i;
        for(int dx{ 0 }; dx <= n.r; ++dx) {
            for(int dy{ 0 }; dx + dy <= n.r; ++dy) {
                assert(mag(*possible_positions.insert({ n.pos.x + dx, n.pos.y + dy, n.pos.z + n.r - dx - dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x + dx, n.pos.y - dy, n.pos.z + n.r - dx - dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x - dx, n.pos.y + dy, n.pos.z + n.r - dx - dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x - dx, n.pos.y - dy, n.pos.z + n.r - dx - dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x + dx, n.pos.y + dy, n.pos.z - n.r + dx + dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x + dx, n.pos.y - dy, n.pos.z - n.r + dx + dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x - dx, n.pos.y + dy, n.pos.z - n.r + dx + dy }).first - n.pos) == n.r);
                assert(mag(*possible_positions.insert({ n.pos.x - dx, n.pos.y - dy, n.pos.z - n.r + dx + dy }).first - n.pos) == n.r);
            }
        }
        /
        /*
        possible_positions.insert({ n.pos.x - n.r, n.pos.y,       n.pos.z       });
        possible_positions.insert({ n.pos.x + n.r, n.pos.y,       n.pos.z       });
        possible_positions.insert({ n.pos.x,       n.pos.y - n.r, n.pos.z       });
        possible_positions.insert({ n.pos.x,       n.pos.y + n.r, n.pos.z       });
        possible_positions.insert({ n.pos.x,       n.pos.y,       n.pos.z - n.r });
        possible_positions.insert({ n.pos.x,       n.pos.y,       n.pos.z + n.r });
        /
    }

    std::cout << "Possibles positions: " << possible_positions.size() << '\n';

    Vec3 max_pos{
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max()
    };
    int max_bots{ 0 };

    for(auto const& p : possible_positions) {
        auto bots = std::count_if(std::begin(ns), std::end(ns), [p] (auto const& n) {
            auto dist = mag(n.pos - p);
            return dist <= n.r;
        });

        if (max_bots < bots || (max_bots == bots && mag(max_pos) > mag(p))) {
            max_bots = bots;
            max_pos = p;
        }
    }

    std::cout << ">> " << max_pos << " with " << max_bots << " bots\n";
    std::cout << ">> " << mag(max_pos) << '\n';
*/
/*
    auto min_it = std::min_element(std::begin(ns), std::end(ns), [] (auto const& l, auto const& r) {
        return l.r < r.r;
    });

    std::cout << "Min range: " << min_it->r << '\n';
*/

    Vec3 max_pos{
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max()
    };
    int max_bots{ 0 };

    for(int x = -160344804; x < 194428994; x += 1944280) {
        for(int y = -60589215; y < 223126255; y += 194428) {
            for(int z = -124893858; z < 162463326; z += 194428) {
                Vec3 p{ x, y, z };
                if (mag(p) <= 120799323) continue;
                auto bots = std::count_if(std::begin(ns), std::end(ns), [p] (auto const& n) {
                    auto dist = mag(n.pos - p);
                    return dist <= n.r;
                });

                if (max_bots < bots || (max_bots == bots && mag(max_pos) > mag(p))) {
                    max_bots = bots;
                    max_pos = p;
                }
            }
        }
    }

    std::cout << ">> " << max_pos << " with " << max_bots << " bots\n";
    std::cout << ">> " << mag(max_pos) << '\n';

}