#include "../common.hpp"

namespace p1 {

}

long part_1(std::vector<char> const& values, int w, int h) {
    long num_0;
    long num_1;
    long num_2;

    long layer = 0;    

    long min_0 = std::numeric_limits<long>::max();
    long _1_times_2 = -1;

    for(auto it = std::begin(values); it != std::end(values); ++layer) {
        num_0 = 0;
        num_1 = 0;
        num_2 = 0;

        for(int h_ = 0; h_ < h; ++h_) {
            for(int w_ = 0; w_ < w; ++w_) {
                assert(it != std::end(values));
                char c = *it++;
                switch(c) {
                    case 0: ++num_0; break;
                    case 1: ++num_1; break;
                    case 2: ++num_2; break;
                    default: break;
                }
            }
        }

        if(min_0 > num_0) {
            min_0 = num_0;
            _1_times_2 = num_1 * num_2;
        }
    }

    return _1_times_2;
}

namespace p2 {

}

long part_2(std::vector<char> const& values, int w, int h) {
    std::vector<char> image(w*h,2);

    long layer = 0;    

    for(auto it = std::begin(values); it != std::end(values); ++layer) {
        auto image_it = std::begin(image);

        for(int h_ = 0; h_ < h; ++h_) {
            for(int w_ = 0; w_ < w; ++w_) {
                assert(it != std::end(values));
                char c = *it++;
                if(*image_it == 2) {
                    *image_it = c;
                }

                ++image_it;
            }
        }
    }

    auto image_it = std::begin(image);

    for(int h_ = 0; h_ < h; ++h_) {
        for(int w_ = 0; w_ < w; ++w_) {
            switch(*image_it++) {
                case 0: std::cout << '.'; break;
                case 1: std::cout << '#'; break;
                default: std::cout << ' '; break;
            }
        }
        std::cout << '\n';
    }

    return -1;
}

int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return digits_of(file);
    }();
    int w = 25;
    int h = 6;


    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(values, w, h);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(values, w, h);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values, w, h));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values, w, h));
        }
    }

    {
        PROFILE_PART(1);
        part_1(values, w, h);
    }

    {
        PROFILE_PART(2);
        part_2(values, w, h);
    }
}