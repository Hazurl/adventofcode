#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <variant>

#include <cpp-tools/dbg.hpp>
#include <cpp-tools/profiler.hpp>
#include <tinge/tinge.hpp>

std::vector<std::string> read_file(std::filesystem::path const& path) {
    PROFILE_FUNCTION();

    std::ifstream file(path);
    if (!file.is_open()) {
        tinge::errorln("Couldn't open file '", path, "'");
        throw std::runtime_error("Couldn't opent file '" + path.string() + "'");
    }

    std::string line;
    std::vector<std::string> lines;
    while(std::getline(file, line)) {
        if (!line.empty()) {
            lines.emplace_back(std::move(line));
        }
    }

    return lines;
}

template<typename...Ts, typename...Rs>
std::tuple<Ts...> extract_(std::istream& is, std::index_sequence<>, Rs&&...rs) {
    return std::tuple<Ts...>(std::forward<Rs>(rs)...);
}

template<typename...Ts, std::size_t I, std::size_t...Is, typename...Rs>
std::tuple<Ts...> extract_(std::istream& is, std::index_sequence<I, Is...>, Rs&&...rs) {
    using T = std::tuple_element_t<I, std::tuple<Ts...>>;
    T value;
    is >> value;
    return extract_(is, std::index_sequence<Is...>{}, std::forward<Rs>(rs)..., std::move(value));
}

template<typename...Ts>
std::tuple<Ts...> extract(std::istream& is) {
    return extract_(is, std::make_index_sequence<sizeof...(Ts)>{});
}