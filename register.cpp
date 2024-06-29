#include "register.h"

int Register::registers = 0;
Register* Register::regs = new Register[32];

Register::Register() {
    num = registers ++;
    offset = 0;
}

std::string Register::to_string() {
    if (num == 29) return "$sp";
    else if (num == 30) return "$fp";
    return "$" + std::to_string(num);
}

std::string Register::location_sp() {
    return std::to_string(offset) + "($sp)";
}

Register* Register::get_register(int i) {
    return &regs[i];
}