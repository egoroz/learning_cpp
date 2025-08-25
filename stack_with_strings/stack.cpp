#include <iostream>
#include <cstring>

struct Stack {
    char** data;
    size_t sz;
    size_t cap;

    Stack() : data(nullptr), sz(0), cap(0) {}

    ~Stack() {
        for (size_t i = 0; i < sz; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }

    void push(const char* str) {
        if (cap == 0) {
            cap = 8;
            data = new char*[cap];
        } else if (sz == cap) {
            cap *= 2;
            char** new_data = new char*[cap];
            std::memcpy(new_data, data, sz * sizeof(char*));
            delete[] data;
            data = new_data;
        }
        data[sz] = new char[std::strlen(str) + 1];
        std::strcpy(data[sz++], str);
        std::cout << "ok\n";
    }

    void pop() {
        if (sz == 0) {
            std::cout << "error\n";
            return;
        }
        std::cout << data[--sz] << "\n";
        delete[] data[sz];
    }

    void back() {
        if (sz == 0) {
            std::cout << "error\n";
            return;
        }
        std::cout << data[sz - 1] << "\n";
    }

    void size() {
        std::cout << sz << "\n";
    }

    void clear() {
        for (size_t i = 0; i < sz; ++i) {
            delete[] data[i];
        }
        sz = 0;
        std::cout << "ok\n";
    }
};

int main() {
    Stack s;
    char command[16];

    while (true) {
        std::cin >> command;
        if (std::strcmp(command, "push") == 0) {
            char buffer[1024];
            std::cin >> buffer;
            s.push(buffer);
        } else if (std::strcmp(command, "pop") == 0) {
            s.pop();
        } else if (std::strcmp(command, "back") == 0) {
            s.back();
        } else if (std::strcmp(command, "size") == 0) {
            s.size();
        } else if (std::strcmp(command, "clear") == 0) {
            s.clear();
        } else if (std::strcmp(command, "exit") == 0) {
            std::cout << "bye\n";
            break;
        } else {
            std::cout << "unknown command\n";
            break;
        }
    }

    return 0;
}

