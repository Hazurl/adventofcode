#include "../common.hpp"

namespace p1 {

using stop_t = bool;

stop_t process_once(std::vector<long>& program, std::size_t& pc) {
    long (*fun)(long, long) = nullptr;

    switch(program[pc++]) {
        case 99: return true;
        default: return false;

        case 1: fun = [] (long a, long b) { return a + b; }; break;
        case 2: fun = [] (long a, long b) { return a * b; }; break;
    }

    long  lhs =  program[program[pc++]];
    long  rhs =  program[program[pc++]];
    long* res = &program[program[pc++]];

    *res = fun(lhs, rhs);

    return false;
}

long process_with_noun_and_verb(std::vector<long> const& values, long noun, long verb) {
    auto program = values;

    // Restore gravity assist program to the "1202 program alarm"
    program[1] = noun;
    program[2] = verb;

    std::size_t pc = 0;
    while(!p1::process_once(program, pc));

    return program[0];
}


}

long part_1(std::vector<long> const& values) {
    return p1::process_with_noun_and_verb(values, 12, 2);
}

namespace p2 {

template<long...Is>
constexpr auto all_solutions_for(std::integer_sequence<long, Is...>) {
    return std::array<std::pair<long, long>, sizeof...(Is)>{ std::make_pair(Is / 100, Is % 100)... };
}

constexpr static auto all_solutions = all_solutions_for(std::make_integer_sequence<long, 100*100>{});

}

long part_2(std::vector<long> const& values) {
    long const solution = 19690720;

    for(long noun = 0; noun < 100; ++noun) {
        for(long verb = 0; verb < 100; ++verb) {
            if (solution == p1::process_with_noun_and_verb(values, noun, verb)) {
                return noun * 100 + verb;
            }
        }
    }

    std::cerr << "No solution\n";
    return -1;
}

long part_2_unseq_par(std::vector<long> const& values) {
    long const solution = 19690720;
    auto it = std::find_if(std::execution::par_unseq, std::begin(p2::all_solutions), std::end(p2::all_solutions), [&values, solution] (auto s) {
        auto&&[noun, verb] = s;
        return solution == p1::process_with_noun_and_verb(values, noun, verb);
    });

    if (it == std::end(p2::all_solutions)) {
        std::cerr << "No solution\n";
        return -1;
    }

    return it->first * 100 + it->second;
}


int main() {
    PROFILE_FUNCTION();

    auto values = [] (){
        PROFILE_SCOPE("Reading file");
        auto file = open_file("input.txt");
        return comma_separated<long>(file);
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

            {
                PROFILE_SCOPE("Part 2 unseq_par");
                part_2_unseq_par(values);
            }
        }
    }

    {
        PROFILE_SCOPE("Result");
        {
            PROFILE_SCOPE("Part 1");
            tinge::println("[Part 1] Value of #0: ", part_1(values));
        }

        {
            PROFILE_SCOPE("Part 2");
            tinge::println("[Part 2] Noun and verb: ", part_2(values));
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

    {
        PROFILE_PART(2_unseq_par);
        part_2_unseq_par(values);
    }
}