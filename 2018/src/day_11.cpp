#include <array>
#include <string>
#include <iostream>

int power_level(int x, int y) {
    constexpr int grid_serial = 1133;

    auto rack_id = x + 10;
    auto total = (rack_id * y + grid_serial) * rack_id;
    auto digit = (total % 1000) / 100;

    return digit - 5;
}

int main() {
    std::array<std::array<int, 300>, 300> power { 0 };

    long max{ 0 };
    long max_x{ 0 };
    long max_y{ 0 };
    long max_size{ 0 };

    for(int x{299}; x >= 0; --x) {
        for(int y{299}; y >= 0; --y) {
            power[x][y] = power_level(x, y);

            long adding{ 0 };
            long last{ 0 };

            int size = 1;
            while(x + size <= 300 && y + size <= 300) {
                for(int dx{x}; dx < x + size - 1; ++dx) {
                    last += power[dx][y + size - 1];
                }
                for(int dy{y}; dy < y + size; ++dy) {
                    last += power[x + size - 1][dy];
                }

                if (last > max) {
                    max = last;
                    max_x = x;
                    max_y = y;
                    max_size = size;
                }

                ++size;
            }
        }
    }

    std::cout << max_x << "," << max_y << "," << max_size << " with " << max << '\n';
}