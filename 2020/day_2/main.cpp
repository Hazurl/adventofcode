#include "../common.hpp"

namespace p1 {

struct Row {
    long min;
    long max;
    char char_requirement;

    std::string password;
};

Row read_row(std::string const& line) {
    std::stringstream ss(line);

    Row row;
    ss >> row.min;
    assert('-' == ss.get());
    ss >> row.max >> row.char_requirement;
    assert(':' == ss.get());
    assert(' ' == ss.get());
    ss >> row.password;

    return row;
}

}

long part_1(std::vector<std::string> const& lines) {
    long count = 0;

    for(auto const& line : lines) {
        auto row = p1::read_row(line);

        auto count_char = std::count(std::begin(row.password), std::end(row.password), row.char_requirement);

        if (count_char >= row.min && count_char <= row.max) {
            ++count;
        }
    }

    return count;
}

namespace p2 {
struct Row {
    long idx0;
    long idx1;
    char char_requirement;

    std::string password;
};

Row read_row(std::string const& line) {
    std::stringstream ss(line);

    Row row;
    ss >> row.idx0;
    assert('-' == ss.get());
    ss >> row.idx1 >> row.char_requirement;
    assert(':' == ss.get());
    assert(' ' == ss.get());
    ss >> row.password;

    return row;
}
}

long part_2(std::vector<std::string> const& lines) {
    long count = 0;

    for(auto const& line : lines) {
        auto row = p2::read_row(line);

        auto char_in_idx0 = row.password[row.idx0 - 1] == row.char_requirement;
        auto char_in_idx1 = row.password[row.idx1 - 1] == row.char_requirement;

        if (char_in_idx0 != char_in_idx1) {
            ++count;
        }
    }

    return count;
}

int main() {
    PROFILE_FUNCTION();

    auto lines = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return lines_of(file);
    }();

    {
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
        }
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