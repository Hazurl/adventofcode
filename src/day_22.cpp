#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include <optional>
#include <unordered_set>

struct Input {
    std::size_t depth;
    std::size_t target_x;
    std::size_t target_y;
};

constexpr Input input {
    5616,
    10, 785
};

enum class Type {
    Rocky, Wet, Narrow
};

Type calc_type(std::size_t erosion) {
    switch(erosion % 3) {
        case 0: return Type::Rocky;
        case 1: return Type::Wet;
        case 2: return Type::Narrow;
    }

    std::cerr << "calc_type :: wtf...\n";
    std::exit(1);
}

std::vector<std::vector<Type>> create_map(Input const& input) {
    std::size_t const width = input.target_x + 200;
    std::size_t const height = input.target_y + 200;

    std::vector<std::vector<std::size_t>> erosions;

    for(std::size_t y{ 0 }; y < height; ++y) {
        erosions.emplace_back(std::initializer_list<std::size_t>{ (y * 48271 + input.depth) % 20183 });
    }

    {
        auto& row = erosions.at(0);
        for(std::size_t x{ 1 }; x < width; ++x) {
            row.emplace_back((x * 16807 + input.depth) % 20183);
        }
    }

    for(std::size_t y{ 1 }; y < height; ++y) {
        auto& row = erosions.at(y);
        for(std::size_t x{ 1 }; x < width; ++x) {
            if (y == input.target_y && x == input.target_x) {
                row.emplace_back(0);
            } else {
                row.emplace_back((erosions[y][x - 1] * erosions[y - 1][x] + input.depth) % 20183);
            }
        }
    }

    std::vector<std::vector<Type>> map(height, std::vector<Type>(width));

    for(std::size_t y{ 0 }; y < height; ++y) {
        for(std::size_t x{ 0 }; x < width; ++x) {
            map[y][x] = calc_type(erosions[y][x]);
        }
    }

    return map;
}

std::size_t risk_level(Type ty) {
    switch(ty) {
        case Type::Rocky: return 0;
        case Type::Wet: return 1;
        case Type::Narrow: return 2;
    }

    std::cerr << "risk_level :: wtf...\n";
    std::exit(1);
}

std::size_t risk_level(std::vector<std::vector<Type>> const& map, Input const& input) {
    std::size_t total{ 0 };
    for(std::size_t y{ 0 }; y <= input.target_y; ++y) {
        for(std::size_t x{ 0 }; x <= input.target_x; ++x) {
            total += risk_level(map[y][x]);
        }
    }

    return total;
}

enum class Tool {
    Torch, ClimbingGear, None
};

bool tool_allow(Type in, Tool tool) {
    switch(in) {
        case Type::Rocky: return tool != Tool::None;
        case Type::Wet: return tool != Tool::Torch;
        case Type::Narrow: return tool != Tool::ClimbingGear;
    }

    std::cerr << "tool_allow :: wtf...\n";
    std::exit(1);
}

bool can_move(Type from, Type to, Tool tool) {
    return tool_allow(from, tool) && tool_allow(to, tool);
}

struct Step {
    std::size_t x;
    std::size_t y;

    Tool tool;

    std::size_t time;
    std::size_t prediction;
    std::size_t tool_changed;

    std::size_t lx;
    std::size_t ly;
    Tool lt;
};

bool operator < (Step const& l, Step const& r) {
    return l.prediction + l.time < r.prediction + r.time;
}

bool operator > (Step const& l, Step const& r) {
    return l.prediction + l.time > r.prediction + r.time;
}

bool operator == (Step const& l, Step const& r) {
    return l.x == r.x && l.y == r.y && l.tool == r.tool;
}

std::optional<Step> next_step(std::vector<std::vector<Type>> const& map, Input const& input, Step const& current, std::size_t x, std::size_t y, Tool tool) {

    if (y >= map.size() || x >= map.at(y).size()) {
        return std::nullopt;
    }

    Type cur_type = map.at(current.y).at(current.x);
    Type next_type = map.at(y).at(x);

    if (!can_move(cur_type, next_type, tool)) {
        return std::nullopt;
    }

    long const idx = static_cast<long>(input.target_x) - static_cast<long>(x);
    long const idy = static_cast<long>(input.target_y) - static_cast<long>(y);

    long const dx = static_cast<long>(current.x) - static_cast<long>(x);
    long const dy = static_cast<long>(current.y) - static_cast<long>(y);

    if (static_cast<std::size_t>(std::abs(dx)) + static_cast<std::size_t>(std::abs(dy)) > 1) {
        std::cerr << "next_step :: wtf...\n";
        std::exit(1);
    }

    return Step {
        x, y, tool,
        current.time + (tool == current.tool ? 0 : 7) + static_cast<std::size_t>(std::abs(dx)) + static_cast<std::size_t>(std::abs(dy)),
        static_cast<std::size_t>(std::abs(idx)) + static_cast<std::size_t>(std::abs(idy)) + static_cast<std::size_t>(tool != Tool::Torch ? 7 : 0),
        current.tool_changed + (tool == current.tool ? 0 : 1),
        current.x, current.y, current.tool
    };
}

struct Hasher {
    std::size_t operator() (Step const& p) const {
        return (p.x << 1) ^ (p.y << 5) ^ (p.y >> 3) ^ static_cast<std::size_t>(p.tool);
    }
};

Tool next_tool(Tool t) {
    switch(t) {
        case Tool::None: return Tool::ClimbingGear; 
        case Tool::ClimbingGear: return Tool::Torch; 
        case Tool::Torch: return Tool::None; 
    }
    std::cerr << "next_tool :: wtf...\n";
    std::exit(1);
}

Tool previous_tool(Tool t) {
    switch(t) {
        case Tool::None: return Tool::Torch; 
        case Tool::ClimbingGear: return Tool::None; 
        case Tool::Torch: return Tool::ClimbingGear; 
    }
    std::cerr << "previous_tool :: wtf...\n";
    std::exit(1);
}

std::size_t fastest_way(std::vector<std::vector<Type>> const& map, Input const& input) {
    std::priority_queue<Step, std::vector<Step>, std::greater<Step>> steps;
    steps.push({ 0, 0, Tool::Torch, 0, input.target_x + input.target_y, 0, 0, 0, Tool::Torch});

    std::size_t mx{ 0 };
    std::size_t my{ 0 };

    std::unordered_set<Step, Hasher> past_steps {
        //steps.top()
    };

    auto check_and_add = [&map, &input, &steps, &past_steps] (Step const& current, std::size_t x, std::size_t y, Tool tool) {
        auto res = next_step(map, input, current, x, y, tool);

        if (!res) {
            return;
        }

        //auto it = past_steps.find(*res);

        if (true/*it == std::end(past_steps) || res->time < it->time*/) {
            /*if (it != std::end(past_steps)) {
                past_steps.erase(it);
            }*/
            //past_steps.insert(*res);
            //std::cout << " + " << res->x << ", " << res->y << " with " << (int)(res->tool) << " : " << res->time << " / " << res->prediction << '\n';
            steps.push(*res);
        }
    };

    while(!steps.empty()) {
        auto step = steps.top();
        steps.pop();

        auto it = past_steps.find(step);

        if (it != std::end(past_steps) && step.time >= it->time) {
            continue;
        }

        if (it != std::end(past_steps)) {
            past_steps.erase(it);
        }

        past_steps.insert(step);



        mx = std::max(step.x, mx);
        my = std::max(step.y, my);

        //std::cout << "> " << step.x << ", " << step.y << " with " << (int)(step.tool) << " : " << step.time << " / " << step.prediction << '\n';

        if (step.x == input.target_x && step.y == input.target_y && step.tool == Tool::Torch) {
            std::cout << "# " << step.time << " = " << step.tool_changed * 7 << " (tools) + " << step.time - (step.tool_changed * 7) << " (walking)\n";
/*
            auto x = step.x;
            auto y = step.y;
            auto t = step.tool;
            while(x != 0 || y != 0 || t != Tool::Torch) {
                std::cout << "(" << x << ", " << y << " : " << (int)t << ") <- ";
                Step fake_step{ x, y, t, 0, 0, 0, 0, 0, Tool::Torch };
                auto it = past_steps.find(fake_step);
                x = it->lx;
                y = it->ly;
                t = it->lt;
            }

            std::cout << "(0, 0 : 0)\n";

            std::cout << "max: (" << mx << ", " << my << ")\n";
*/
            return step.time;
            continue;
        }

        check_and_add(step, step.x + 1, step.y, step.tool);
        check_and_add(step, step.x - 1, step.y, step.tool);
        check_and_add(step, step.x, step.y + 1, step.tool);
        check_and_add(step, step.x, step.y - 1, step.tool);

        check_and_add(step, step.x, step.y, next_tool(step.tool));
        check_and_add(step, step.x, step.y, previous_tool(step.tool));

        //std::cin.get();
    }

    std::cerr << "fastest_way :: wtf...\n";
    std::exit(1);
}

int main() {
    //Input i { 510, 10, 10 }; 
    auto i = input;
    auto map = create_map(i);
    std::cout << ">> " << risk_level(map, i) << '\n';
    std::cout << ">> " << fastest_way(map, i) << '\n';
}