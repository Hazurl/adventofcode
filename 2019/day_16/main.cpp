#include "../common.hpp"

namespace p1 {

long sum(std::vector<int> const& values, std::size_t n) {
    auto it = std::begin(values);
    auto end = std::end(values);

    std::size_t m = n <= 1 ? 0 : 1;
    int s = n <= 1 ? 1 : 0;

    int r = 0;

    while(it != end) {
        if (s == 1) r += *it; 
        else if (s == 3) r -= *it; 

        ++it;
        ++m;
        if (m >= n) {
            m = 0;
            ++s;
            if (s >= 4) s = 0;
        }
    }

    return r;
}

int digit_sum(std::vector<int> const& values, std::size_t n) {
    int s = sum(values, n);
    return std::abs(s) % 10;
}

}

long part_1(std::string const& input) {
    std::vector<int> current;
    std::vector<int> next(input.size());
    current.reserve(input.size());

    for(auto c : input) {
        current.push_back(c - '0');
    }

    for(int i = 0; i < 100; ++i) {

        for(int n = 0; n < current.size(); ++n) {
            next[n] = p1::digit_sum(current, n + 1);
        }

        std::swap(current, next);
        //DBG(current);
    }

    long r = 0;
    for(int i = 0; i < 8; ++i) {
        r = r * 10 + current[i];
    }

    return r;
}

namespace p2 {

long sum(std::vector<int> const& values, std::size_t n) {
    std::size_t i = 0;
    std::size_t l = values.size();

    int r = 0;

    i += n - 1;

    while(i < l) {
        for(std::size_t m = i + n; i < m && i < l; ++i) {
            r += values[i];
        }
        i += n;
        for(std::size_t m = i + n; i < m && i < l; ++i) {
            r -= values[i];
        }
        i += n;
    }

    return r;
}

int digit_sum(std::vector<int> const& values, std::size_t n) {
    int s = sum(values, n);
    return std::abs(s) % 10;
}

}

long part_2(std::string const& input) {
    /*
    std::vector<int> current;
    std::vector<int> next(input.size() * 10000);
    current.reserve(input.size() * 10000);

    long to_skip = 0;
    for(int i = 0; i < 7; ++i) {
        to_skip = to_skip * 10 + current[i];
    }
    for(int i = 0; i < 10000; ++i) {
        for(auto c : input) {
            current.push_back(c - '0');
        }
    }
    std::cout << "Processing... " << current.size() << '\n';
    for(int i = 0; i < 100; ++i) {

        for(int n = 0; n < current.size(); ++n) {
            next[n] = p1::digit_sum(current, n + 1);
            if (n % 1000 == 0) std::cout << "> " << n << '\n';
        }
        std::cout << '\n';

        std::swap(current, next);
        //DBG(current);
    }

    long r = 0;
    for(int i = 0; i < 8; ++i) {
        r = r * 10 + current[to_skip + i];
    }

    return r;*/
    std::vector<int> current;
    std::vector<int> next(input.size());
    current.reserve(input.size());

    for(auto c : input) {
        current.push_back(c - '0');
    }

    for(int i = 0; i < 100; ++i) {

        for(int n = 0; n < current.size(); ++n) {
            next[n] = p2::digit_sum(current, n + 1);
        }

        std::swap(current, next);
        //DBG(current);
    }

    long r = 0;
    for(int i = 0; i < 8; ++i) {
        r = r * 10 + current[i];
    }

    return r;

}

int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file)[0];
    }();


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
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1]: ", part_1(values));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2]: ", part_2(values));
        }
    }

    {
        PROFILE_PART(1);
        part_1(values);
    }

    {
        PROFILE_PART(2);
        part_2(values);
    }
}