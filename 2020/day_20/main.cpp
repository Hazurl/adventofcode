#include "../common.hpp"

template<std::size_t N>
std::bitset<N> flip_bitset(std::bitset<N> bits) {
    std::bitset<N> res;
    for(std::size_t i = 0; i < bits.size(); ++i) {
        res[res.size() - 1 - i] = bits[i];
    }

    return res;
}

template<std::size_t N>
std::bitset<N> flip_image(std::bitset<N> image, std::size_t width) {
    std::bitset<N> final_image;
    auto height = N / width;
    for(std::size_t i = 0; i < height; ++i) {
        for(std::size_t j = 0; j < width; ++j) {
            final_image[i * width + (width - 1 - j)] = image[i * width + j];
        }
    }

    return final_image;
}

template<std::size_t N>
std::bitset<N> rotate_image(std::bitset<N> image, std::size_t width) {
    std::bitset<N> final_image;
    auto height = N / width;
    for(std::size_t i = 0; i < height; ++i) {
        for(std::size_t j = 0; j < width; ++j) {
            final_image[j * height + (height - 1 - i)] = image[i * width + j];
        }
    }

    return final_image;
}

struct Borders {
    std::bitset<10> up;
    std::bitset<10> down;
    std::bitset<10> left;
    std::bitset<10> right;

    void flip() {
        std::swap(left, right);
        up = flip_bitset(up);
        down = flip_bitset(down);
        left = flip_bitset(left);
        right = flip_bitset(right);
    }

    void rotate_cw() {
        auto tmp = up;
        up = left;
        left = down;
        down = right;
        right = tmp;
    }

    bool operator==(Borders const& other) const {
        return other.up == up && other.down == down && other.left == left && other.right == right;
    }
};

struct Tile {
    long id;
    std::bitset<10> up_border;
    std::bitset<10> down_border;
    std::bitset<10> left_border;
    std::bitset<10> right_border;

    std::bitset<100> image_with_border;
    std::bitset<64> image;

    bool used = false;
    int rotation_cw = 0;
    bool flip = false;

    Borders get_borders() const {
        Borders borders{
            up_border, 
            down_border, 
            left_border,
            right_border,
        };

        if (flip) {
            borders.flip();
        }
        for(int r = 0; r < rotation_cw; ++r) {
            borders.rotate_cw();
        }

        return borders;
    }

    std::bitset<64> get_image() const {
        std::bitset<64> final_image = image;
        if(flip) {
            final_image = flip_image(image, 8);
        }

        for(int r = 0; r < rotation_cw; ++r) {
            final_image = rotate_image(final_image, 8);
        }
        return final_image;
    }
};


std::vector<Tile> parse_tiles(std::vector<std::string> const& lines) {
    std::vector<Tile> tiles;

    for(std::size_t i = 0; i < lines.size(); ++i) {
        auto const& header = lines[i];
        Tile tile;
        tile.id = std::strtol(header.c_str() + 5, nullptr, 10);

        for(std::size_t j = 0; j < 10; ++j) {
            auto const& line = lines[i + j + 1];

            for(std::size_t k = 0; k < 10; ++k) {
                tile.image_with_border[j * 10 + k] = line[k] == '#';
            }
        }

        i += 10;

        for(std::size_t j = 0; j < 10; ++j) {
            tile.up_border[j] = tile.image_with_border[j];
            tile.down_border[9 - j] = tile.image_with_border[90 + j];
            tile.left_border[9 - j] = tile.image_with_border[j * 10];
            tile.right_border[j] = tile.image_with_border[j * 10 + 9];
        }

        for(std::size_t j = 0; j < 8; ++j) {
            for(std::size_t k = 0; k < 8; ++k) {
                tile.image[j * 8 + k] = tile.image_with_border[(j + 1) * 10 + (k + 1)];
            }
        }

        tiles.emplace_back(std::move(tile));
    }

    return tiles;
}

bool border_match(std::bitset<10> lhs, std::bitset<10> rhs) {
    return lhs == flip_bitset(rhs);
}

Tile* find_matching_tile(std::vector<Tile>& tiles, long tile_id, std::bitset<10> border, std::bitset<10> Borders::* opp_border) {
    for(auto& tile : tiles) {
        if (tile.id == tile_id) continue;
        auto borders = tile.get_borders();
        auto flipped_borders = borders;
        flipped_borders.flip();

        for(int rotation = 0; rotation < 4; ++rotation) {
            if (rotation > 0) {
                borders.rotate_cw();
                flipped_borders.rotate_cw();
            }

            if (border_match(border, borders.*opp_border)) {
                tile.rotation_cw = rotation;
                tile.flip = false;
                return &tile;
            }

            if (border_match(border, flipped_borders.*opp_border)) {
                tile.rotation_cw = rotation;
                tile.flip = true;
                return &tile;
            }
        }
    }

    return nullptr;
}

bool has_any_matching_tile(std::vector<Tile> const& tiles, long tile_id, std::bitset<10> border) {
    long count = 0;
    auto flipped = flip_bitset(border);

    for(auto const& tile : tiles) {
        if (tile.id == tile_id) continue;

        if (
            border_match(tile.up_border, border) || border_match(tile.up_border, flipped) ||
            border_match(tile.down_border, border) || border_match(tile.down_border, flipped) ||
            border_match(tile.left_border, border) || border_match(tile.left_border, flipped) ||
            border_match(tile.right_border, border) || border_match(tile.right_border, flipped)
        ) {
            ++count;
        }
    }

    return count > 0;
}

bool is_edge(std::vector<Tile> const& tiles, long tile_id, std::bitset<10> border) {
    return !has_any_matching_tile(tiles, tile_id, border);
}

Tile* find_top_left_corner(std::vector<Tile>& tiles) {
    for(auto& tile : tiles) {
        auto borders = tile.get_borders();
        auto flipped_borders = borders;
        flipped_borders.flip();

        for(int rotation = 0; rotation < 4; ++rotation) {
            if (rotation > 0) {
                borders.rotate_cw();
                flipped_borders.rotate_cw();
            }

            if (is_edge(tiles, tile.id, borders.up) && is_edge(tiles, tile.id, borders.left)) {
                tile.rotation_cw = rotation;
                tile.flip = false;
                return &tile;
            }

            if (is_edge(tiles, tile.id, flipped_borders.up) && is_edge(tiles, tile.id, flipped_borders.left)) {
                tile.rotation_cw = rotation;
                tile.flip = true;
                return &tile;
            }
        }
    }

    return nullptr;
}

Tile* find_matching_tile_right_of(std::vector<Tile>& tiles, long tile_id, std::bitset<10> border) {
    return find_matching_tile(tiles, tile_id, border, &Borders::left);
}

Tile* find_matching_tile_down_of(std::vector<Tile>& tiles, long tile_id, std::bitset<10> border) {
    return find_matching_tile(tiles, tile_id, border, &Borders::up);
}

enum class Pixel {
    Empty,
    Dragon,
    Fill
};

void fill_image(Pixel* top_left, std::size_t line_offset, Tile const& tile) {
    auto image = tile.get_image();
    for(std::size_t i = 0; i < 8; ++i) {
        for(std::size_t j = 0; j < 8; ++j) {
            top_left[i * line_offset + j] = image[i * 8 + j] ? Pixel::Fill : Pixel::Empty;
        }
    }
}

template<std::size_t N>
void print_image(std::bitset<N> image, std::size_t width) {
    for(std::size_t y = 0; y < image.size() / width; ++y) {
        for(std::size_t x = 0; x < width; ++x) {
            std::cout << (image[y * width + x] ? '#' : '.');
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

template<std::size_t N>
void toggle_dragon(std::vector<Pixel>& image, std::size_t image_width, std::bitset<N> pattern, std::size_t pattern_width) {
    auto const image_height = image.size() / image_width;
    auto const pattern_height = pattern.size() / pattern_width;

    for(std::size_t y = 0; y < image_height - pattern_height; ++y) {
        for(std::size_t x = 0; x < image_width - pattern_width; ++x) {

            bool valid = true;

            for(std::size_t dy = 0; dy < pattern_height; ++dy) {
                for(std::size_t dx = 0; dx < pattern_width; ++dx) {
                    if (pattern[dy * pattern_width + dx] && image[(y + dy) * image_width + (x + dx)] == Pixel::Empty) {
                        valid = false;
                        break;
                    }
                }

                if (!valid) break;
            }

            if (valid) {
                std::cout << "Found at " << x << ", " << y << '\n';
                for(std::size_t dy = 0; dy < pattern_height; ++dy) {
                    for(std::size_t dx = 0; dx < pattern_width; ++dx) {
                        auto& pixel = image[(y + dy) * image_width + (x + dx)];
                        if (pattern[dy * pattern_width + dx] && pixel != Pixel::Empty) {
                            pixel = Pixel::Dragon;
                        }
                    }
                }
            }

        }
    }
}

long part_1(std::vector<std::string> const& lines) {
    auto tiles = parse_tiles(lines);

    long product_corner = 1;

    for(auto const& tile : tiles) {
        long edges_count = 0;
        edges_count += is_edge(tiles, tile.id, tile.up_border);
        edges_count += is_edge(tiles, tile.id, tile.down_border);
        edges_count += is_edge(tiles, tile.id, tile.left_border);
        edges_count += is_edge(tiles, tile.id, tile.right_border);

        if (edges_count == 2) {
            product_corner *= tile.id;
        }
    }

    return product_corner;
}

long part_2(std::vector<std::string> const& lines) {
    auto tiles = parse_tiles(lines);

    std::vector<Tile*> ordered_tiles(tiles.size());
    std::size_t row_count = 0;

    // Find first corner
    ordered_tiles[0] = find_top_left_corner(tiles);
    ordered_tiles[0]->used = true;
    assert(ordered_tiles[0]);
    assert(is_edge(tiles, ordered_tiles[0]->id, ordered_tiles[0]->get_borders().up));
    assert(is_edge(tiles, ordered_tiles[0]->id, ordered_tiles[0]->get_borders().left));
    assert(!is_edge(tiles, ordered_tiles[0]->id, ordered_tiles[0]->get_borders().right));
    assert(!is_edge(tiles, ordered_tiles[0]->id, ordered_tiles[0]->get_borders().down));

    // Fill first row
    ++row_count;
    Tile* last_tile = ordered_tiles[0];
    while(true) {
        auto tile = find_matching_tile_right_of(tiles, last_tile->id, last_tile->get_borders().right);
        if (tile == nullptr) break;

        ordered_tiles[row_count++] = tile;
        tile->used = true;
        last_tile = tile;

        assert(is_edge(tiles, tile->id, tile->get_borders().up));
    }

    assert(is_edge(tiles, ordered_tiles[row_count - 1]->id, ordered_tiles[row_count - 1]->get_borders().right));

    std::size_t column_count = tiles.size() / row_count; 
    std::cout << "Rows count: " << row_count << '\n';
    std::cout << "Columns count: " << column_count << '\n';

    // Fill other rows
    for(std::size_t row_index = 1; row_index < column_count; ++row_index) {
        for(std::size_t i = 0; i < row_count; ++i) {
            auto& tile_above = *ordered_tiles[(row_index - 1) * row_count + i];
            auto border_down = tile_above.get_borders().down;
            auto tile = find_matching_tile_down_of(tiles, tile_above.id, border_down);

            assert(tile);
            ordered_tiles[row_index * row_count + i] = tile;
            tile->used = true;
        }
    }

    for(std::size_t i = 0; i < row_count; ++i) {
        for(std::size_t j = 0; j < column_count; ++j) {
            std::cout << ordered_tiles[i * row_count + j]->id << " ";
        }
        std::cout << '\n';
    }

    std::vector<Pixel> image(row_count * 8 * column_count * 8);
    std::size_t const image_width = row_count * 8;
    std::size_t const image_height = column_count * 8;
    for(std::size_t i = 0; i < row_count; ++i) {
        for(std::size_t j = 0; j < column_count; ++j) {
            auto ix = j * 8;
            auto iy = i * 8;
            fill_image(image.data() + image_width * iy + ix, image_width, *ordered_tiles[i * row_count + j]);
        }
    }

    for(std::size_t y = 0; y < image_height; ++y) {
        for(std::size_t x = 0; x < image_width; ++x) {
            std::cout << (image[y * image_width + x] == Pixel::Empty ? '.' : '#');
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    char const dragon_str[] = "                  # #    ##    ##    ### #  #  #  #  #  #   ";
    constexpr std::size_t dragon_size = std::size(dragon_str) - 1;
    constexpr std::size_t dragon_height = 3;
    constexpr std::size_t dragon_width = std::size(dragon_str) / dragon_height;
    std::bitset<dragon_size> dragon;

    for(std::size_t x = 0; x < dragon.size(); ++x) {
        dragon[x] = dragon_str[x] == '#';
    }

    using dragon_pattern_t = std::pair<std::bitset<dragon_size>, std::size_t>;
    std::array all_dragons{
        dragon_pattern_t{ dragon, dragon_width },
        dragon_pattern_t{ rotate_image(dragon, dragon_width), dragon_height },
        dragon_pattern_t{ rotate_image(rotate_image(dragon, dragon_width), dragon_height), dragon_width },
        dragon_pattern_t{ rotate_image(rotate_image(rotate_image(dragon, dragon_width), dragon_height), dragon_width), dragon_height },
        dragon_pattern_t{}, dragon_pattern_t{}, dragon_pattern_t{}, dragon_pattern_t{}
    };

    for(std::size_t i = 0; i < 4; ++i) {
        all_dragons[i + 4] = { flip_image(all_dragons[i].first, all_dragons[i].second), all_dragons[i].second };
    }
    int ii = 0;
    for(auto[pattern, width] : all_dragons) {
        std::cout << "For dragon #" << ii++ << '\n';
        toggle_dragon(image, image_width, pattern, width);
    }
    /*
    std::cout << "Dragon #0\n";
    print_image(all_dragons[0].first, all_dragons[0].second);
    std::cout << "Dragon #1\n";
    print_image(all_dragons[1].first, all_dragons[1].second);
    std::cout << "Dragon #4\n";
    print_image(all_dragons[4].first, all_dragons[4].second);

    auto p = all_dragons[1];
    toggle_dragon(image, image_width, p.first, p.second);
*/
    for(std::size_t y = 0; y < image.size() / image_width; ++y) {
        for(std::size_t x = 0; x < image_width; ++x) {
            std::cout << (image[y * image_width + x] == Pixel::Empty ? '.' : (image[y * image_width + x] == Pixel::Dragon ? 'O' : '#'));
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    return std::count(std::begin(image), std::end(image), Pixel::Fill);
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();
    {/*
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
        }*/
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