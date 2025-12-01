# 2025 Advent of code (C++)

Get cpp-tools and tinge from github:
```
git clone https://github.com/Jackojc/tinge.git
git clone https://github.com/Hazurl/cpp-tools.git
```
Depending on where you clone you might have to change the the include path `-I` in the compile command below.

Compiled with:
```
cd dayXX
g++ -std=c++2c -Werror -Wno-error=unused-parameter main.cpp -I../../.. -Wall -Wextra
./a.out
```

Running a day will generate a flamegraph profiler file which you can open in `chrome://tracing/`.