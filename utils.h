#ifndef UTILS
#define UTILS

#include <cstdio>
#include <iostream>

namespace mips {

inline void binary_register(const char* op, std::string lhs, std::string rhs, std::string save, const char* from = "") {
  std::printf("\t%s %s, %s, %s\t\t# %s\n", op, save.c_str(), lhs.c_str(), rhs.c_str(), from);
}

inline void binary_imm(const char* op, std::string save, int rhs, const char* from = "") {
  std::printf("\t%s %s, %d\t\t# %s\n", op, save.c_str(), rhs, from);
}

inline void unary_register(const char* op, std::string save, const char* from = "") {
  std::printf("\t%s %s\t\t# %s\n", op, save.c_str(), from);
}

inline void binary_div(const char* op, const char* op2, std::string lhs, std::string rhs, std::string save, const char* from = "") {
  std::printf("\t%s %s, %s\t\t# %s\n", op, lhs.c_str(), rhs.c_str(), from);
  std::printf("\t%s %s\n", op2, save.c_str());
}

// save and load register
inline void access(const char* op, const std::string reg, const std::string stk, const char* from = "") {
  std::printf("\t%s %s, %s\t\t# %s\n", op, reg.c_str(), stk.c_str(), from);
}

// free and allocate memory
inline void manage(const char* op, const int size, const char* from = "") {
    std::printf("\t%s $sp %d\t\t# %s\n", op, size, from);
}

inline void system_print(const std::string x, const char* from = "") {
  access("li", "$v0", "1");
  std::printf("\tsyscall\n");
  access("li", "$v0", "4");
  access("la", "$a0", "newline");
  std::printf("\tsyscall\n");
}

inline void return_value(const std::string x, const char* from = "") {
  std::printf("\tjr $31\t\t\t# %s\n", from);
}

inline void jump(const std::string jump, const std::string x, const char* from = "") {
    std::printf("\t%s %s\t\t# %s\n", jump.c_str(), x.c_str(), from);
}

inline void label(const std::string x, const char* from = "") {
    std::printf("\t%s:\t\t# %s\n", x.c_str(), from);
}

inline void conditional_jump(const std::string x, int d, const std::string y, const std::string z, const char* from = "") {
    mips::binary_register("sltu", "$0", x, x);
    mips::access("li", y, std::to_string(d));
    std::printf("beq %s, %s, %s\t\t# %s\n", x.c_str(), y.c_str(), z.c_str(), from);
}

}

#endif