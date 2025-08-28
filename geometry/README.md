g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror \
    -Wconversion -Wsign-conversion -Wshadow -Wdouble-promotion \
    -fno-omit-frame-pointer \
    tests.cpp -lgtest -lgtest_main -lpthread

valgrind ./a.out