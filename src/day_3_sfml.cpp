/*
    Visual representation of the fabric
*/

#include <SFML/Graphics.hpp>
#include "../formatted_inputs/day_3.inc"

#include <array>
#include <vector>
#include <cassert>
#include <string>
#include <sstream>
#include <cmath>
#include <iostream>

struct Entry {
    unsigned id;
    unsigned left;
    unsigned top;
    unsigned width;
    unsigned height;
};

Entry parse(std::string_view const& s) {
    std::stringstream is(std::string{s});

    Entry entry;

    assert(is.get() == '#');

    is >> entry.id;

    assert(is.get() == ' ');
    assert(is.get() == '@');
    assert(is.get() == ' ');

    is >> entry.left;
    assert(is.get() == ',');
    is >> entry.top;

    assert(is.get() == ':');
    assert(is.get() == ' ');

    is >> entry.width;
    assert(is.get() == 'x');
    is >> entry.height;

    return entry;
}

sf::Color get_color(unsigned value) {
    if (value == 0) return sf::Color::Black;

    value--;

    constexpr float max = 7;

    assert(value <= max);

    int t = static_cast<float>(value) * 360.f / max;

    int step = static_cast<unsigned>(std::floor(t / 60.f)) % 6;
    
    float f = t / 60.f - static_cast<float>(step);
    sf::Uint8 l = 0;
    sf::Uint8 m = static_cast<sf::Uint8>((1 - f) * 255);
    sf::Uint8 n = f * 255;

    switch(step) {
        case 0:  return sf::Color{255, n, l};
        case 1:  return sf::Color{m, 255, l};
        case 2:  return sf::Color{l, 255, n};
        case 3:  return sf::Color{l, m, 255};
        case 4:  return sf::Color{n, l, 255};
        default: return sf::Color{255, l, m};
    }
}

void update(sf::Texture& texture, std::size_t index, std::array<std::array<unsigned, 1000>, 1000>& fabric) {
    auto entry = parse(input[index]);

    std::vector<sf::Uint8> area(entry.width * entry.height * 4);

    std::size_t i{ 0 };

    for(unsigned y{entry.top}; y < entry.top + entry.height; ++y) {
        for(unsigned x{entry.left}; x < entry.left + entry.width; ++x) {
            auto color = get_color(++fabric[x][y]);
            area.at(i++) = color.r;
            area.at(i++) = color.g;
            area.at(i++) = color.b;
            area.at(i++) = color.a;
        }
    }


    texture.update(area.data(), entry.width, entry.height, entry.left, entry.top);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "[SFML] AdventOfCode Day 3");
    window.setPosition({400, 50});

    std::size_t index = 0;
    std::array<std::array<unsigned, 1000>, 1000> fabric { 0 };

    sf::Texture texture;
    texture.setSmooth(true);
    texture.create(1000, 1000);

    sf::Sprite sprite(texture);

    sf::Shader blur;
    blur.loadFromFile("res/blur_shader.glsl", sf::Shader::Fragment);
    blur.setUniform("texture", sf::Shader::CurrentTexture);


    bool start = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                start = !start;
            }
        }

        window.clear();
        for(int i = 0; i < 10; ++i) {
            if(index < input.size() && start) {
                update(texture, index++, fabric);
            }
        }
        window.draw(sprite, &blur);
        window.display();
    }

    return 0;
}