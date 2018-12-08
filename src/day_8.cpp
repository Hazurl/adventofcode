#include "../formatted_inputs/day_8.inc"

#include <iostream>
#include <stack>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <numeric>
#include <algorithm>

long read_child_part_1(std::istream& ss) {
    int childs, metas;
    if (!(ss >> childs)) { std::cout << "error \n"; std::exit(1); };
    if (!(ss >> metas)) { std::cout << "error \n"; std::exit(1); };


    long total{0};

    for(; childs > 0; --childs)
        total += read_child_part_1(ss);

    for(; metas > 0; --metas) {
        int tmp;
        ss >> tmp;
        total += tmp;
    }

    return total;
}

long part_1(std::istream& ss) {
    std::stack<std::pair<int, int>> headers;

    long total{0};
    std::pair<int, int> header { 1, 0 };
    headers.push(header);

    while(!headers.empty()) {
        --headers.top().first;
        ss >> header.first;
        ss >> header.second;
        headers.push(header);

        while(!headers.empty() && headers.top().first <= 0) {
            for(int meta{headers.top().second}; meta > 0; --meta) {
                int tmp;
                ss >> tmp;
                total += tmp;
            }
            headers.pop();
        }
    }
    return total;
}

long read_child_part_2(std::istream& ss) {
    int childs, metas;
    if (!(ss >> childs)) { std::cout << "error \n"; std::exit(1); };
    if (!(ss >> metas)) { std::cout << "error \n"; std::exit(1); };

    long total{0};


    if(childs == 0) {
        for(; metas > 0; --metas) {
            int tmp;
            ss >> tmp;
            total += tmp;
        }
    } else {
        std::vector<long> values(childs, 0);
        std::generate(std::begin(values), std::end(values), [&ss] () { return read_child_part_2(ss); });

        for(; metas > 0; --metas) {
            int tmp;
            ss >> tmp;
            std::size_t index = tmp - 1;
            if (tmp != 0 && index < values.size())
                total += values.at(index);
        }
    }

    return total;
}

int main() {

    std::ifstream file("inputs/day_8");
    std::cout << part_1(file) << '\n';

    file.seekg(0, std::ios::beg);
    std::cout << read_child_part_1(file) << '\n'; // recursive version


    file.seekg(0, std::ios::beg);

    file.seekg(0, std::ios::beg);
    std::cout << read_child_part_2(file) << '\n'; // recursive version
}