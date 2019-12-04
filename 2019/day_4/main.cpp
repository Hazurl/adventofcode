#include "../common.hpp"

namespace p1 {

using password_t = std::array<long, 6>;

password_t increase(password_t p, std::size_t i) {
    ++p[i];

    if (p[i] >= 10) {
        if (i + 1 < 6) {
            p = increase(p, i+1);
            p[i] = p[i+1]; 
        }
    }

    return p;
}

password_t next(password_t p) {
    return increase(p, 0);
}

password_t to_password(long r) {
    return {
        r / 1 % 10,
        r / 10 % 10,
        r / 100 % 10,
        r / 1000 % 10,
        r / 10000 % 10,
        r / 100000 % 10
    };
}

password_t first_password(password_t p) {
    for(std::size_t i = 5; i > 0; --i) {
        if (p[i] > p[i-1]) {
            auto v = p[i];
            std::fill(std::begin(p), std::begin(p) + i, v);
            return p;
        }
    }
    return p;
}

bool is_password(password_t const& p) {
    for(std::size_t i = 0; i < 5; ++i) {
        assert(p[i] >= p[i+1]);
        if (p[i] == p[i+1]) return true;
    }
    return false;
}

bool greater(password_t const& lhs, password_t const& rhs) {
    return lhs[5] > rhs[5] || lhs[4] > rhs[4] || lhs[3] > rhs[3] || lhs[2] > rhs[2] || lhs[1] > rhs[1] || lhs[0] > rhs[0];
}

}

long part_1(long lower_bound, long upper_bound) {
    auto pw = p1::first_password(p1::to_password(lower_bound));
    auto upper_pw = p1::to_password(upper_bound);
    long count = 0;
    while(p1::greater(upper_pw, pw)) {
        if (p1::is_password(pw)) {
            ++count;
        }
        pw = p1::next(pw);
    }
    return count;
}

namespace p2 {

bool is_password(p1::password_t const& p) {
    for(std::size_t i = 0; i < 5; ++i) {
        assert(p[i] >= p[i+1]);
        if (p[i] == p[i+1] && !(i+2 <= 5 && p[i+2] == p[i]) && !(i > 0 && p[i-1] == p[i])) {
            return true;
        }
    }
    return false;
}

}

long part_2(long lower_bound, long upper_bound) {
    auto pw = p1::first_password(p1::to_password(lower_bound));
    auto upper_pw = p1::to_password(upper_bound);
    long count = 0;
    while(p1::greater(upper_pw, pw)) {
        if (p2::is_password(pw)) {
            ++count;
        }
        pw = p1::next(pw);
    }
    return count;
}

int main() {
    PROFILE_FUNCTION();

    long lower_bound = 240920;
    long upper_bound = 789857;

    {
        PROFILE_SCOPE("Caching");
        for(int _ = 0; _ < 3; ++_) {
            {
                PROFILE_SCOPE("Part 1");
                part_1(lower_bound, upper_bound);
            }

            {
                PROFILE_SCOPE("Part 2");
                part_2(lower_bound, upper_bound);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1] Minimum distance: ", part_1(lower_bound, upper_bound));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2] Minimum steps: ", part_2(lower_bound, upper_bound));
        }
    }

    {
        PROFILE_PART(1);
        part_1(lower_bound, upper_bound);
    }

    {
        PROFILE_PART(2);
        part_2(lower_bound, upper_bound);
    }
}