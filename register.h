#pragma once
#include <iostream>
class Register {
private:
    int num;
public:
    int offset;
    static int registers;
    static Register* regs;
    static Register* get_register(int i);
public:
    Register();
    std::string to_string();
    std::string location_sp();
};