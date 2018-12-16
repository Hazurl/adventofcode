#include "../formatted_inputs/day_15.inc"

#include <algorithm>
#include <vector>
#include <iostream>
#include <optional>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_set>

enum class Cell {
    Empty,
    Wall
};

using Map = std::array<std::array<Cell, input[0].size()>, input.size()>;
using PathMap = std::array<std::array<std::size_t, input[0].size()>, input.size()>;

struct Entity {
    bool is_elf;
    std::size_t x;
    std::size_t y;

    std::size_t id;

    int health{ 200 };
};

struct Combat {
    Map map;
    std::vector<Entity> entities;
};

Combat parse() {
    Combat c;

    for(std::size_t y{ 0 }; y < input.size(); ++y) {
        auto const& row = input[y];
        for(std::size_t x{ 0 }; x < row.size(); ++x) {

            char ch = row[x];

            c.map[y][x] = ch == '#' ? Cell::Wall : Cell::Empty;

            if (ch == 'G') {
                c.entities.push_back({
                    false, x, y, c.entities.size()
                });
            }
            else if (ch == 'E') {
                c.entities.push_back({
                    true, x, y, c.entities.size()
                });
            }
        }
    }

    return c;
}

std::vector<std::pair<std::size_t, std::size_t>> adgacent_cells(std::size_t x, std::size_t y) {
    std::vector<std::pair<std::size_t, std::size_t>> cells;
    if (y > 0) {
        auto cx = x;
        auto cy = y - 1;
        cells.push_back({cx, cy});
    }

    if (x > 0) {
        auto cx = x - 1;
        auto cy = y;
        cells.push_back({cx, cy});
    }

    if (x < input[0].size() - 1) {
        auto cx = x + 1;
        auto cy = y;
        cells.push_back({cx, cy});
    }

    if (y < input.size() - 1) {
        auto cx = x;
        auto cy = y + 1;
        cells.push_back({cx, cy});
    }
    return cells;
}

std::vector<std::pair<std::size_t, std::size_t>> empty_adgacent_cells(Map const& map, std::size_t x, std::size_t y) {
    auto cells = adgacent_cells(x, y);
    cells.erase(std::remove_if(std::begin(cells), std::end(cells), [&map] (auto c) { return map[c.second][c.first] == Cell::Wall; }), std::end(cells));
    return cells;
}

std::optional<Entity*> in_range(std::size_t x, std::size_t y, std::vector<Entity*>& entities, std::unordered_set<Entity*> const& dead) {
    std::optional<Entity*> res;
    for(auto&&[cx, cy] : adgacent_cells(x, y)) {
        auto e = std::find_if(std::begin(entities), std::end(entities), [cx, cy, &dead] (Entity* e) {
            return dead.count(e) == 0 && e->x == cx && e->y == cy;
        });
        if (e != std::end(entities) && (!res || (*res)->health > (*e)->health)) {
            res = *e;
        }
    }

    return res;
}

std::optional<std::pair<std::pair<std::size_t, std::size_t>, std::size_t>> pathfinding_helper(Map const& map, std::pair<std::size_t, std::size_t> const& from, std::pair<std::size_t, std::size_t> const& to) {
    PathMap values;
    for(auto& r : values) r.fill(std::numeric_limits<std::size_t>::max());

    auto firsts_cells = empty_adgacent_cells(map, to.first, to.second);
    auto cell_comparator = [&values, &from] (std::pair<std::size_t, std::size_t> const& l, std::pair<std::size_t, std::size_t> const& r) {
        return 
            (values[l.second][l.first] + 
                std::abs(static_cast<int>(from.first) - static_cast<int>(l.first)) + 
                std::abs(static_cast<int>(from.second) - static_cast<int>(l.second))) > 
            (values[r.second][r.first] + 
                std::abs(static_cast<int>(from.first) - static_cast<int>(r.first)) + 
                std::abs(static_cast<int>(from.second) - static_cast<int>(r.second)));
    };

    values[to.second][to.first] = 1;

    std::priority_queue<std::pair<std::size_t, std::size_t>, std::vector<std::pair<std::size_t, std::size_t>>, decltype(cell_comparator)> 
        queue(cell_comparator);

    queue.push(to);

    while(!queue.empty()) {
        auto next = queue.top();
        queue.pop();
        auto value = values[next.second][next.first];
        auto next_adgacents = empty_adgacent_cells(map, next.first, next.second);

        for(auto it = std::rbegin(next_adgacents); it != std::rend(next_adgacents); ++it) {
            auto& adgacent = *it;

            if (adgacent == from) {
                return {{ next, value }};
            }
            if (values[adgacent.second][adgacent.first] > value + 1) {
                values[adgacent.second][adgacent.first] = value + 1;
                queue.push(adgacent);
            }
        }
    }

    return std::nullopt;
}

std::optional<std::pair<std::pair<std::size_t, std::size_t>, std::size_t>> pathfinding(Map const& map, std::pair<std::size_t, std::size_t> const& from, std::pair<std::size_t, std::size_t> const& to) {
    std::optional<std::pair<std::pair<std::size_t, std::size_t>, std::size_t>> res = {};

    bool debug = from == std::pair<std::size_t, std::size_t>{10, 21}; 

    if(debug) std::cout << "####\n";
    for(auto c : empty_adgacent_cells(map, from.first, from.second)) {
        if (debug)
            std::cout << " {" << c.first << ", " << c.second << " => " << to.first << ", " << to.second << "}";
        if (c == to) {
            if (debug)
                std::cout << " (0 step)";
            return {{c, 0}};
        }
        auto p = pathfinding_helper(map, c, to);
        if (p) {
            if (debug)
                std::cout << " (" << p->second << " steps)";

            if (!res || res->second > p->second) {
                if (debug) 
                std::cout << " >> replace it\n";
                res = { c, p->second };
            } else if (debug) std::cout << '\n';
        }

    }

    return res;
}

std::optional<std::pair<std::size_t, std::size_t>> find_cell_to_attack(std::size_t x, std::size_t y, std::vector<Entity*> const& entities, Map const& map, std::unordered_set<Entity*> const& dead) {
    std::optional<std::pair<std::size_t, std::size_t>> cell;
    std::size_t min_dist = 0;
    std::pair<std::size_t, std::size_t> aim;

    for(auto e : entities) {
        if (dead.count(e)) continue;
        for(auto&&[cx, cy] : empty_adgacent_cells(map, e->x, e->y)) {

            if (!cell) {
                auto path = pathfinding(map, {x, y}, {cx, cy});
                if (path) {
                    cell = path->first;
                    aim = {cx, cy};
                    min_dist = path->second;
                }
            } else {
                auto path = pathfinding(map, {x, y}, {cx, cy});
                if (path) {
                    if (min_dist > path->second) {
                        cell = path->first;
                        aim = {cx, cy};
                        min_dist = path->second;
                    } else if (min_dist == path->second) {
                        if (cy < aim.second || (cy == aim.second && cx < aim.first)) {
                            cell = path->first;
                            aim = {cx, cy};
                            min_dist = path->second;
                        }
                    }
                }
            }
        }
    }

    std::cout << "(Aiming " << aim.first << ", " << aim.second << ")";
    return cell;
}

bool play_round(Map const& map, std::vector<Entity>& entities, int elf_attack, bool& more) {
    auto comparator = [] (Entity const& l, Entity const& r) {
        if (l.y == r.y) return l.x < r.x;
        return l.y < r.y;
    };

    std::sort(std::begin(entities), std::end(entities), comparator);

    std::vector<Entity*> goblins;
    std::vector<Entity*> elves;
    for(auto& e : entities) {
        if (e.is_elf) {
            elves.emplace_back(&e);
        } else {
            goblins.emplace_back(&e);
        }
    }

    for(auto& entity : entities) {
        std::cout << (entity.is_elf ? "Elf" : "Goblin") << " at " << entity.x << ", " << entity.y << " #" << entity.id << " with " << entity.health << "pv\n";
    }
    std::cout << '\n';

    std::unordered_set<Entity*> dead;

    auto path_map = map;
    for(auto e : entities) {
        path_map[e.y][e.x] = Cell::Wall;
    }

    for(auto& entity : entities) {

        if (dead.count(&entity)) {
            continue;
        }

        if (goblins.empty()) {
            std::cout << ".Elves won!\n";

            entities.erase(std::remove_if(std::begin(entities), std::end(entities), [&dead] (auto& e) {
                return dead.count(&e);
            }), std::end(entities));
            return false;
        }

        if (elves.empty()) {
            std::cout << ".Goblins won!\n";

            entities.erase(std::remove_if(std::begin(entities), std::end(entities), [&dead] (auto& e) {
                return dead.count(&e);
            }), std::end(entities));
            return true;
        }

        std::cout << (entity.is_elf ? "Elf" : "Goblin") << " at " << entity.x << ", " << entity.y << " #" << entity.id << " with " << entity.health << "pv ";
        if (auto c = in_range(entity.x, entity.y, entity.is_elf ? goblins : elves, dead); c) {
            std::cout << "\nAttack " << (*c)->x << ", " << (*c)->y << " => " << (*c)->health - 3 << '\n';
            if (((*c)->health -= (entity.is_elf ? elf_attack : 3)) < 0) {
                path_map[(*c)->y][(*c)->x] = Cell::Empty;
                //std::cout << "\n\n\t######## SET " << (*c)->x << " " << (*c)->y << " : " << (path_map[(*c)->y][(*c)->x] == Cell::Empty ? "Empty" : "Wall/Entity") << '\n';
                dead.insert(*c);
                std::cout << "###################### Dead\n";
                if (!entity.is_elf) { more = true; return true; }
                auto* es = entity.is_elf ? &goblins : &elves;
                es->erase(std::find(std::begin(*es), std::end(*es), *c));
            }
        } else {
            //std::cout << "\n\n\t######## AT 18 22 : " << (path_map[22][18] == Cell::Empty ? "Empty" : "Wall/Entity") << '\n';
            auto cell = find_cell_to_attack(entity.x, entity.y, entity.is_elf ? goblins : elves, path_map, dead);
            path_map[entity.y][entity.x] = Cell::Empty;
            if (cell) {
                std::cout << "Go to " << cell->first << ", " << cell->second << '\n';
                entity.x = cell->first;
                entity.y = cell->second;
                if (auto c = in_range(entity.x, entity.y, entity.is_elf ? goblins : elves, dead); c) {
                    std::cout << "Attack " << (*c)->x << ", " << (*c)->y << " => " << (*c)->health - 3 << '\n';
                    if (((*c)->health -= (entity.is_elf ? elf_attack : 3)) < 0) {
                        path_map[(*c)->y][(*c)->x] = Cell::Empty;
                        //std::cout << "\n\n\t######## SET " << (*c)->x << " " << (*c)->y << " : " << (path_map[(*c)->y][(*c)->x] == Cell::Empty ? "Empty" : "Wall/Entity") << '\n';
                        dead.insert(*c);
                        std::cout << "###################### Dead\n";
                        if (!entity.is_elf) { more = true; return true; }
                        auto* es = entity.is_elf ? &goblins : &elves;
                        es->erase(std::find(std::begin(*es), std::end(*es), *c));
                    }
                }
            } else {
                std::cout << "Can't find any path :(\n";
            }
            path_map[entity.y][entity.x] = Cell::Wall;
        }
    }

    entities.erase(std::remove_if(std::begin(entities), std::end(entities), [&dead] (auto& e) {
        return dead.count(&e);
    }), std::end(entities));

    if (true || !dead.empty()) {
        std::array<std::string, input.size()> m;
        for(std::size_t y{ 0 }; y < m.size(); ++y) {
            auto& row = m[y];
            for(std::size_t x{ 0 }; x < input[0].size(); ++x) {
                row.push_back(map[y][x] == Cell::Empty ? '.' : '#');
            }
        }

        for(auto const& e : entities) {
            m[e.y][e.x] = e.is_elf ? 'E' : 'G';
        }

        std::cout << '\n';
        for(auto const& s : m) {
            std::cout << s << '\n';
        }
        //std::cin.get();
    }
    
/*
    {
        std::vector<Entity*> goblins;
        std::vector<Entity*> elves;
        for(auto& e : entities) {
            if (e.is_elf) {
                elves.emplace_back(&e);
            } else {
                goblins.emplace_back(&e);
            }
        }

        if (goblins.empty()) {
            std::cout << "Elves won!\n";

            return false;
        }

        if (elves.empty()) {
            std::cout << "Goblins won!\n";
            return false;
        }
    }
*/
    return true;
}

int main() {
    auto perm = parse();

    auto map = perm.map; 
    auto entities = perm.entities; 

    int dam = 4;
    for(int i = 1;; ++i) {
        std::cout << i << '\n';
        bool more = false;
        if (!play_round(map, entities, dam, more)) {
            for(auto e : entities) {
                std::cout << "=> " << e.health << '\n';
            }
            auto sum = std::accumulate(std::begin(entities), std::end(entities), 0, [] (int s, auto const& e) { return s + e.health; });
            std::cout << "Sum: " << sum << '\n';
            std::cout << "Result: " << sum * (i - 1) << '\n';
            std::cout << "Damages: " << dam << '\n';
            return 0;
        }

        if (more) {
            std::cout << "\n\nRESET\n\n";
            more = false;
            ++dam;
            i = 0;
            map = perm.map; 
            entities = perm.entities; 
        }
        std::cout << '\n' << '\n';
    }
}