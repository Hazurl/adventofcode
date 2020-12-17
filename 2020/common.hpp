#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <cassert>
#include <type_traits>
#include <variant>
#include <unordered_set>
#include <deque>
#include <regex>

#include <cpp-tools/dbg.hpp>
#include <cpp-tools/profiler.hpp>
#include <tinge/tinge.hpp>

#define ALWAYS_PROFILE(str) \
auto _watch_ ## __LINE__ = prf::scoped_watch_of_precision<std::chrono::nanoseconds>([] (auto d) { \
    tinge::println_h(str, ": ", d.count() / 1000'000., "ms"); \
}); \

#define PROFILE_PART(n) ALWAYS_PROFILE("Part " #n); PROFILE_SCOPE("Part " #n)

std::vector<std::string> lines_of(std::istream& is, bool remove_empty_lines = true) {
    PROFILE_FUNCTION();

    std::string line;
    std::vector<std::string> lines;
    while(std::getline(is, line)) {
        if (!remove_empty_lines || !line.empty()) {
            lines.emplace_back(std::move(line));
        }
    }

    return lines;
}

std::vector<char> digits_of(std::istream& is) {
    PROFILE_FUNCTION();
    std::vector<char> digits;

    char cur = is.peek();
    while(cur >= '0' && cur <= '9') {
        digits.push_back(is.get() - '0');
        cur = is.peek();
    }

    return digits;
}

std::vector<std::string> split(std::istream& is, char del) {
    std::vector<std::string> strs;
    std::string str;

    while(std::getline(is, str, del)) {
        strs.emplace_back(std::move(str));
    }

    return strs;
}


template<typename T>
std::vector<T> comma_separated(std::istream& is) {
    std::vector<T> values;

    T value;
    while(is >> value) {
        values.emplace_back(std::move(value));

        if (is.peek() == ',') {
            is.get();
        } else {
            break;
        }
    }

    return values;
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

std::ifstream open_file(std::filesystem::path const& path) {
    PROFILE_FUNCTION();

    std::ifstream file(path);
    if (!file.is_open()) {
        tinge::errorln("Couldn't open file '", path, "'");
        throw std::runtime_error("Couldn't opent file '" + path.string() + "'");
    }

    return file;
}

