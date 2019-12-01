#include "../formatted_inputs/day_17.inc"

#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <cassert>

#include <SFML/Graphics.hpp>

struct hasher {
    std::size_t operator() (std::pair<int, int> const& p) const { 
        return static_cast<std::size_t>(p.first ^ p.second); 
    }
};

struct View {
    int x;
    int y;
    int width;
    int height;
};

struct ClayPatch {
    int x;
    int y;
    int width;
    int height;
};

ClayPatch parse(std::string_view const& s) {
    std::stringstream is(std::string{s});
    ClayPatch patch{0, 0, 0, 0};

    int* axis = is.get() == 'x' ? &patch.x : &patch.y;
    assert(is.get() == '=');
    is >> *axis;
    (axis == &patch.x ? patch.width : patch.height) = *axis;

    assert(is.get() == ',');
    assert(is.get() == ' ');

    axis = is.get() == 'x' ? &patch.x : &patch.y;
    assert(is.get() == '=');
    is >> *axis;
    assert(is.get() == '.');
    assert(is.get() == '.');
    is >> (axis == &patch.x ? patch.width : patch.height);

    patch.width -= patch.x;
    patch.height -= patch.y;

    return patch;
}

std::array<ClayPatch, input.size()> parse_input() {
    std::array<ClayPatch, input.size()> patches;
    std::transform(std::begin(input), std::end(input), std::begin(patches), parse);
    return patches;
}

View find_view(std::array<ClayPatch, input.size()> const& patches) {
    View v{patches[0].x, patches[0].y, patches[0].width, patches[0].height};
    for(auto const& p : patches) {
        //std::cout << p.x << ".." << p.width << ", " << p.y << ".." << p.height << '\n';
        v.x = std::min(p.x - 2, v.x);
        v.y = std::min(p.y, v.y);

        v.width = std::max(p.x + p.width + 3, v.x + v.width) - v.x;
        v.height = std::max(p.y + p.height + 1, v.y + v.height) - v.y;
        //std::cout << v.x << ".." << v.width << ", " << v.y << ".." << v.height << '\n';
        //std::cin.get();
    }

    //v.width -= v.x - 1;
    //v.height -= v.y - 1;

    return v;
}

enum class Type {
    Clay, Sand, DrySand, Water
};

std::vector<std::vector<Type>> create_map(std::array<ClayPatch, input.size()> const& patches, View const& view) {
    std::vector<std::vector<Type>> map(view.height, std::vector<Type>(view.width, Type::Sand));

    for(auto const& patch : patches) {
        for(int y{ patch.y }; y <= patch.y + patch.height; ++y) {
            for(int x{ patch.x }; x <= patch.x + patch.width; ++x) {
                map.at(y - view.y).at(x - view.x) = Type::Clay;
            }
        }
    }

    return map;
}

void update_texture(std::vector<std::vector<Type>> const& map, sf::Texture& txt, std::unordered_set<std::pair<int, int>, hasher> const& waters) {
    std::vector<sf::Uint8> pixels(map.size() * map[0].size() * 4, 0);

    std::size_t px{ 0 };
    for(std::size_t y{ 0 }; y < map.size(); ++y) {
        auto& map_row = map[y];
        for(std::size_t x{ 0 }; x < map_row.size(); ++x) {
            /*if (waters.count({x, y})) {
                pixels[px++] = 137;
                pixels[px++] = 11;
                pixels[px++] = 212;
                pixels[px++] = 255;
                continue;
            }*/
            switch(map_row[x]) {
                case Type::Clay:
                    pixels[px++] = 61;
                    pixels[px++] = 78;
                    pixels[px++] = 96;
                    pixels[px++] = 255;
                    break;

                case Type::Sand:
                    pixels[px++] = 221;
                    pixels[px++] = 209;
                    pixels[px++] = 109;
                    pixels[px++] = 255;
                    break;

                case Type::DrySand:
                    pixels[px++] = 172;
                    pixels[px++] = 124;
                    pixels[px++] = 45;
                    pixels[px++] = 255;
                    break;

                case Type::Water:
                    pixels[px++] = 9;
                    pixels[px++] = 50;
                    pixels[px++] = 190;
                    pixels[px++] = 255;
                    break;
            }
        }
    }

    txt.update(pixels.data());
    return;
}

int main() {
    auto patches = parse_input();
    auto view = find_view(patches);

    view.height += view.y - 1;
    view.y = 1;

    std::cout << view.x << ".." << view.width << ", " << view.y << ".." << view.height << '\n';
    //std::cin.get();

    auto map = create_map(patches, view);
    std::cout << "[" << map.size() << ", " << map[0].size() << "]\n";

    sf::View window_view({0.f, 0.f, 1200.f, 800.f});

    sf::RenderWindow window(sf::VideoMode(1200, 800), "[SFML] AdventOfCode Day 17");
    window.setPosition({(1920 - 1200) / 2, (1080 - 800) / 2});

    window.setView(window_view);

    sf::Texture texture;
    if (!texture.create(view.width, view.height)) return 1;

    sf::Sprite sprite(texture);
    sprite.setPosition({600, 400});


    std::unordered_set<std::pair<int, int>, hasher> waters(8);
    waters.insert({ 500 - view.x, -1 });
    std::unordered_set<std::pair<int, int>, hasher> to_add(8);

    update_texture(map, texture, waters);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { 
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    for(auto const& w : waters) {
                        //std::cout << w.first << ", " << w.second << '\n';
                        int dy{ 1 };
                        if (w.second + dy >= static_cast<int>(map.size())) continue;
                        for(; w.second + dy < static_cast<int>(map.size()) && map.at(w.second + dy).at(w.first) != Type::Clay; ++dy) {
                            map.at(w.second + dy).at(w.first) = Type::DrySand;
                        }
                        if (w.second + dy >= static_cast<int>(map.size())) continue;

                        bool raise = true;
                        int most_left = 0;
                        int most_right = 0;
                        bool fill = true;
                        while(raise) {
                            --dy;
                            if (w.second + dy >= static_cast<int>(map.size())) { fill = false; break; }
                            std::cout << "LEVEL " << w.first << ", " << w.second + dy << '\n';
                            map.at(w.second + dy).at(w.first) = Type::Water;
                            {
                                int dx{ 0 };
                                std::cout << "# " << w.first << '\n';
                                for(; map.at(w.second + dy).at(w.first + dx - 1) != Type::Clay 
                                    && (map.at(w.second + dy + 1).at(w.first + dx) == Type::Clay || map.at(w.second + dy + 1).at(w.first + dx) == Type::Water); 
                                    --dx) {

                                    std::cout << "  STEP LEFT (" << dx << ")\n";
                                    most_left = w.first + dx - 1;
                                    map.at(w.second + dy).at(w.first + dx - 1) = Type::Water;
                                }

                                std::cout << "# " << w.first << '\n';
                                if (map.at(w.second + dy + 1).at(w.first + dx) != Type::Clay && map.at(w.second + dy + 1).at(w.first + dx) != Type::Water) {
                                    std::cout << "> HOLE\n";
                                    std::cout << "## " << w.first << '\n';
                                    if (map.at(w.second + dy + 1).at(w.first + dx) == Type::Sand)
                                        to_add.insert({w.first + dx, w.second + dy});
                                    std::cout << "+ " << w.first + dx << ", " << w.second + dy << '\n';
                                    raise = false;
                                    std::cout << "# " << w.first << '\n';
                                } else {
                                    std::cout << "> WALL\n";
                                    std::cout << "# " << w.first << '\n';
                                }
                            }
                            {
                                int dx{ 0 };
                                std::cout << w.first << ", " << dx << ", " << w.first + dx + 1 << ", " << w.second + dy << '\n';
                                for(;  map.at(w.second + dy).at(w.first + dx + 1) != Type::Clay 
                                    && (map.at(w.second + dy + 1).at(w.first + dx) == Type::Clay || map.at(w.second + dy + 1).at(w.first + dx) == Type::Water); 
                                    ++dx) {

                                    std::cout << "  STEP RIGHT (" << dx << ")\n";
                                    most_right = w.first + dx + 1;
                                    map.at(w.second + dy).at(w.first + dx + 1) = Type::Water;
                                }


                                if (map.at(w.second + dy + 1).at(w.first + dx) != Type::Clay && map.at(w.second + dy + 1).at(w.first + dx) != Type::Water) {
                                    std::cout << "> HOLE\n";
                                    if (map.at(w.second + dy + 1).at(w.first + dx) == Type::Sand)
                                        to_add.insert({w.first + dx, w.second + dy});
                                    std::cout << "+ " << w.first + dx << ", " << w.second + dy << '\n';
                                    raise = false;
                                } else {
                                    std::cout << "> WALL\n";
                                }
                            }
                        }

                        if (fill) {
                            for(; most_left <= most_right; ++most_left) {
                                map.at(w.second + dy).at(most_left) = Type::DrySand;
                            }
                        }
                    }

                    waters = std::move(to_add);
                    to_add.clear();
                    std::cout << "done : " << waters.size() << '\n' << '\n' << '\n'; 

                    if (waters.empty()) {
                        int total{ 0 };
                        for(auto const& r : map) {
                            total += std::count_if(std::begin(r), std::end(r), [] (auto t) {
                                return t == Type::Water || t == Type::DrySand;
                            });
                        }
                        int total2{ 0 };
                        for(auto const& r : map) {
                            total2 += std::count_if(std::begin(r), std::end(r), [] (auto t) {
                                return t == Type::Water;
                            });
                        }

                        std::cout << ">>> " << total << '\n';
                        std::cout << ">>> " << total2 << '\n';
                    }

                    update_texture(map, texture, waters);
                } else if (event.key.code == sf::Keyboard::Left) {
                    window_view.move(-20, 0);
                    window.setView(window_view);
                } else if (event.key.code == sf::Keyboard::Right) {
                    window_view.move(20, 0);
                    window.setView(window_view);
                } else if (event.key.code == sf::Keyboard::Up) {
                    window_view.move(0, -20);
                    window.setView(window_view);
                } else if (event.key.code == sf::Keyboard::Down) {
                    window_view.move(0, 20);
                    window.setView(window_view);
                } else if (event.key.code == sf::Keyboard::A) {
                    window_view.zoom(1.2);
                    window.setView(window_view);
                } else if (event.key.code == sf::Keyboard::Z) {
                    window_view.zoom(0.8);
                    window.setView(window_view);
                }
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}