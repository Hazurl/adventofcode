#include "../common.hpp"

void skip_whitespaces(std::istream& is) {
    while(is.peek() == ' ') is.get();
}

long evaluate(std::istream& is);

long evaluate_term(std::istream& is, long (*eval)(std::istream&)) {
    skip_whitespaces(is);

    if (is.peek() == '(') {
        is.get();
        long res = eval(is);
        assert(is.get() == ')');
        return res;
    }

    long r;
    is >> r;
    return r;
}

long evaluate_same_precedence(std::istream& is) {
    long lhs = evaluate_term(is, &evaluate_same_precedence);
    skip_whitespaces(is);
    while(is.peek() == '+' || is.peek() == '*') {
        auto c = is.get();
        auto r = evaluate_term(is, &evaluate_same_precedence);
        if (c == '+') lhs += r;
        else lhs *= r;
        skip_whitespaces(is);
    }

    return lhs;
}

long evaluate_sum(std::istream& is) {
    long lhs = evaluate_term(is, &evaluate);
    skip_whitespaces(is);
    while(is.peek() == '+') {
        is.get();
        auto r = evaluate_term(is, &evaluate);
        lhs += r;
        skip_whitespaces(is);
    }

    return lhs;
}

long evaluate_product(std::istream& is) {
    long lhs = evaluate_sum(is);
    skip_whitespaces(is);
    while(is.peek() == '*') {
        is.get();
        auto r = evaluate_sum(is);
        lhs *= r;
        skip_whitespaces(is);
    }

    return lhs;
}

long evaluate(std::istream& is) {
    return evaluate_product(is);
}

long part_1(std::vector<std::string> const& lines) {
    long sum = 0;
    for(auto const& line : lines) {
        std::stringstream ss(line);
        long r = evaluate_same_precedence(ss);
        sum += r;
    }

    return sum;
}

long part_2(std::vector<std::string> const& lines) {
    long sum = 0;
    for(auto const& line : lines) {
        std::stringstream ss(line);
        long r = evaluate(ss);
        sum += r;
    }

    return sum;
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