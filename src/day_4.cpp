#include "../formatted_inputs/day_4.inc"

#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <iomanip>
#include <cassert>
#include <unordered_map>

struct Timestamp {
    int year;
    int month;
    int day;
    int hour;
    int minute;
};

std::ostream& operator<<(std::ostream& os, Timestamp const& t) {
    return os << t.year << "-" << t.month << "-" << t.day << " " << t.hour << ":" << t.minute;
}

struct FallAsleep {};
struct WakesUp {};
using Shift = int;

using Action = std::variant<FallAsleep, WakesUp, Shift>;

std::ostream& operator<<(std::ostream& os, Action const& a) {
    if (std::holds_alternative<FallAsleep>(a)) {
        return os << "Falls asleep";
    }

    if (std::holds_alternative<WakesUp>(a)) {
        return os << "Wakes up";
    }

    if (std::holds_alternative<Shift>(a)) {
        return os << "Shift to guard #" << std::get<Shift>(a);
    }

    return os;
}

struct Observation {
    Timestamp timestamp;
    Action action;
};

std::ostream& operator<<(std::ostream& os, Observation const& o) {
    return os << "[" << o.timestamp << "] " << o.action;
}


Observation parse(std::string const& s) {
    std::stringstream ss (s);
    Observation o;

    assert(ss.get() == '[');
    ss >> o.timestamp.year;
    assert(ss.get() == '-');
    ss >> o.timestamp.month;
    assert(ss.get() == '-');
    ss >> o.timestamp.day;
    assert(ss.get() == ' ');
    ss >> o.timestamp.hour;
    assert(ss.get() == ':');
    ss >> o.timestamp.minute;
    assert(ss.get() == ']');
    assert(ss.get() == ' ');
    
    char c = ss.get();
    if (c == 'f') {
        o.action = FallAsleep{};
    } else if (c == 'w') {
        o.action = WakesUp{};
    } else {
        ss.ignore(std::numeric_limits<std::streamsize>::max(), '#');
        Shift shift;
        ss >> shift;
        o.action = shift;
    }

    return o;
}

int main() {
    std::array<Observation, input.size()> observations;
    auto it = std::begin(observations);
    for(auto const& s : input) {
        *it++ = parse(std::string{s});
    }

    std::sort(std::begin(observations), std::end(observations), [] (Observation const& lhs, Observation const& rhs) -> bool {
        return 
            lhs.timestamp.year  != rhs.timestamp.year   ? lhs.timestamp.year   < rhs.timestamp.year  : 
            lhs.timestamp.month != rhs.timestamp.month  ? lhs.timestamp.month  < rhs.timestamp.month : 
            lhs.timestamp.day   != rhs.timestamp.day    ? lhs.timestamp.day    < rhs.timestamp.day   : 
            lhs.timestamp.hour  != rhs.timestamp.hour   ? lhs.timestamp.hour   < rhs.timestamp.hour  : 
                                                          lhs.timestamp.minute < rhs.timestamp.minute;
    });

    std::array<std::unordered_map<int, int>, 60> hour;

    std::unordered_map<int, int> guard_asleep_time;

    int guard;
    int minute_asleep;
    int hour_asleep;
    int p{0};
    for(auto const& o : observations) {
        if (std::holds_alternative<FallAsleep>(o.action)) {
            hour_asleep = o.timestamp.hour;
            minute_asleep = o.timestamp.minute;
        } 
        
        else if (std::holds_alternative<WakesUp>(o.action)) {
            auto start = ((hour_asleep + 1) % 24) * 60 + minute_asleep;
            auto end = ((o.timestamp.hour + 1) % 24) * 60 + o.timestamp.minute;
            auto interval = end - start;


            guard_asleep_time[guard] += interval;
            for(int m = start; m < end; ++m) {
                ++hour[m % 60][guard];
            }
        }

        else {
            guard = std::get<Shift>(o.action);
        }
    }

    std::cout << "Guard that sleep the most is "; 
    auto max = std::max_element(std::begin(guard_asleep_time), std::end(guard_asleep_time),
    [] (auto const& lhs, auto const& rhs) {
        return lhs.second < rhs.second;
    });

    std::cout << max->first << '\n';

    int max_value{0};
    int minute_of_max{0};
    for(std::size_t m{0}; m < 60; ++m) {
        if (hour[m][max->first] > max_value) {
            max_value = hour[m][max->first];
            minute_of_max = m;
        }
    }

    std::cout << " at 00:" << minute_of_max << " with " << max_value << '\n';

    std::cout << "So answer is " << max->first * minute_of_max << '\n';


    auto guard2 = 0;
    auto minute = 0;
    auto total = 0;
    for(int m = 0; m < 60; ++m) {
        auto& min = hour[m];
        for(auto const& p : min) {
            if (total < p.second) {
                guard2 = p.first;
                total = p.second;
                minute = m;
            }
        }
    }

    std::cout << "part2: " << guard2 * minute << '\n';

    return 0;
}