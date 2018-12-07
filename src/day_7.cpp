#include "../formatted_inputs/day_7.inc"

#include <unordered_set>
#include <set>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <iostream>
#include <string>

struct Instruction {
    char goal;
    char dependancy;
};

Instruction parse(std::string const& s) {
    std::stringstream is(s);
    Instruction i;

    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is >> i.dependancy;
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is.ignore(std::numeric_limits<std::streamsize>().max(), ' ');
    is >> i.goal;

    i.goal -= 'A';
    i.dependancy -= 'A';

    return i;
}

std::string part_1(std::array<int, 26> depandancies, std::array<std::unordered_set<char>, 26> rev_depandancies, std::set<char> possibilities) {
    std::string res;

    while(! possibilities.empty()) {
        auto first = *possibilities.begin();
        possibilities.erase(first);
        res.push_back(first + 'A');

        for(auto rd : rev_depandancies[first]) {
            if(--depandancies[rd] == 0) {
                possibilities.insert(rd);
            }
        }
    }

    return res;
}

struct Work {
    int step = -1;
    int time = -1;
};

long part_2(std::array<int, 26> depandancies, std::array<std::unordered_set<char>, 26> rev_depandancies, std::set<char> possibilities) {
    std::array<Work, 5> workers;

    long total{ 0 };

    {
    }

    while( 
        (workers[0].step != -1 || 
         workers[1].step != -1 || 
         workers[2].step != -1 ||
         workers[3].step != -1 || 
         workers[4].step != -1) ||
        !possibilities.empty()
    ) {

        if (!possibilities.empty()) {
            auto it = possibilities.begin();
            for(auto& w : workers) {
                if (w.step == -1) {
                    w.step = *it;
                    w.time = *it + 61;
                    ++it;

                    possibilities.erase(w.step);

                    if (possibilities.empty()) {
                        break;
                    }
                }
            }
        }

        int min{ std::numeric_limits<int>().max() };

        for(auto& w : workers) {
            if (w.step != -1 && w.time < min) {
                min = w.time;
            }
        }

        total += min;

        for(auto& w : workers) {
            if ((w.time -= min) == 0) {
                for(auto rd : rev_depandancies[w.step]) {
                    if(--depandancies[rd] == 0) {
                        possibilities.insert(rd);
                    }
                }

                w = { -1, -1 };
            }
        }
    }


    return total;
}

int main() {
    std::array<int, 26> depandancies { 0 };
    std::array<std::unordered_set<char>, 26> rev_depandancies {};

    std::set<char> possibilities;

    for(auto& s : input) {
        auto i = parse(std::string{s});
        ++depandancies[i.goal];
        possibilities.erase(i.goal);
        rev_depandancies[i.dependancy].insert(i.goal);

        if (depandancies[i.dependancy] == 0) {
            possibilities.insert(i.dependancy);
        }
    }

    std::cout << part_1(depandancies, rev_depandancies, possibilities) << '\n';
    std::cout << part_2(depandancies, rev_depandancies, possibilities) << '\n';
    
}