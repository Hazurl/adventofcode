#include "../formatted_inputs/day_10.inc"

#include <regex>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <string>
#include <SFML/Graphics.hpp>

struct Light {
    int x;
    int y;

    int vx;
    int vy;
};

Light parse(std::string_view const& s) {
    std::regex rgx{ "position=<\\s*(-?\\d*),\\s*(-?\\d*)> velocity=<\\s*(-?\\d*),\\s*(-?\\d*)>" };
    std::smatch matches;
    std::string str{s};
    assert(std::regex_search(str, matches, rgx));

    return {
        std::stoi(matches[1]), std::stoi(matches[2]),
        std::stoi(matches[3]), std::stoi(matches[4])
    };
}

struct View {
    int minx;
    int maxx;
    int miny;
    int maxy;
};

View get_view(std::array<Light, input.size()> const& lights) {
    View view {
        lights[0].x, lights[0].x,
        lights[0].y, lights[0].y
    };
    
    for(auto const& l : lights) {
        if (view.minx > l.x) view.minx = l.x;
        else if (view.maxx < l.x) view.maxx = l.x;

        if (view.miny > l.y) view.miny = l.y;
        else if (view.maxy < l.y) view.maxy = l.y;
    }

    return view;
}


void display(std::array<Light, input.size()> const& lights, View const& v, sf::Texture& texture) {
    std::size_t width = v.maxx - v.minx;
    std::size_t height = v.maxy - v.miny;

    int factor = (width > 1000 || height > 1000) ? std::max(width / 1000, height / 1000) + 1 : 1;

    ++(width /= factor);
    ++(height /= factor);

    std::vector<sf::Uint8> sky(4 * 1000 * 1000, 0);
    for(auto const& l : lights) {
        std::size_t x = (l.x - v.minx) / factor;
        std::size_t y = (l.y - v.miny) / factor;
        std::size_t idx = y * 1000 + x;
        std::size_t sky_idx = idx * 4;

        sky.at(sky_idx    ) = 255;
        sky.at(sky_idx + 1) = 255;
        sky.at(sky_idx + 2) = 255;
        sky.at(sky_idx + 3) = 255;

    }

    texture.update(sky.data());
}

void update(std::array<Light, input.size()>& lights ) {
    for(auto& l : lights) {
        l.x += l.vx;
        l.y += l.vy;
    }
}


int main() {

    std::array<Light, input.size()> lights;
    std::transform(std::begin(input), std::end(input), std::begin(lights), parse);

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "[SFML] AdventOfCode Day 10");
    window.setPosition({400, 50});

    sf::Texture texture;
    if (!texture.create(1000, 1000)) return 1;

    sf::Sprite sprite(texture);

    display(lights, get_view(lights), texture);

    bool start = false;
    long long updated = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                start = !start;
            } else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
                update(lights);
                updated++;
                display(lights, get_view(lights), texture);
                std::cout << updated << "\n";
            } 
        }

        if (start) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                update(lights);
                updated++;
                update(lights);
                updated++;
            }
            update(lights);
            updated++;
            display(lights, get_view(lights), texture);
            //start = false;
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}