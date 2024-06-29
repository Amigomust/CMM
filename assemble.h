#pragma once
#include "utils.h"
#include "node.h"

inline void cmd_add(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("add", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());                  
}
inline void cmd_sub(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("sub", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());             
}

inline void cmd_mul(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("mul", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());             
}

inline void cmd_and(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("and", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());                  
}

inline void cmd_or(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("or", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
                 
}

inline void cmd_xor(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("xor", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
                 
}

inline void cmd_slt(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("slt", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
               
}

inline void cmd_sgt(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("slt", Register::get_register(9)->to_string(), Register::get_register(8)->to_string(), save -> to_string(), prev.c_str());                  
}

inline void cmd_le(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("slt", Register::get_register(9)->to_string(), Register::get_register(8)->to_string(), save -> to_string(), prev.c_str()); 
    mips::binary_imm("xori", save -> to_string(), 1, prev.c_str());                  
}

inline void cmd_ge(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("slt", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
    mips::binary_imm("xori", save->to_string(), 1, prev.c_str()); 
}

inline void cmd_eq(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("xor", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
    mips::binary_imm("sltiu", save->to_string(), 1, prev.c_str()); 
}

inline void cmd_ne(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_register("xor", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
    mips::binary_register("sltu", Register::get_register(8)->to_string(), "$0", Register::get_register(8)->to_string(), prev.c_str()); 
}

inline void cmd_logic_and(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("sltu", Register::get_register(0)->to_string(), Register::get_register(8)->to_string(), Register::get_register(8)->to_string(), prev.c_str()); 
    mips::binary_register("sltu", Register::get_register(0)->to_string(), Register::get_register(9)->to_string(), Register::get_register(9)->to_string(), prev.c_str());
    mips::binary_register("and", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());              
}

inline void cmd_logic_or(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {       
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));
    mips::binary_register("sltu", Register::get_register(0)->to_string(), Register::get_register(8)->to_string(), Register::get_register(8)->to_string(), prev.c_str()); 
    mips::binary_register("sltu", Register::get_register(0)->to_string(), Register::get_register(9)->to_string(), Register::get_register(9)->to_string(), prev.c_str());
    mips::binary_register("or", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str());                          
}

inline void cmd_div(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_div("div", "mflo", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(), save -> to_string(), prev.c_str()); 
}

inline void cmd_mod(Expr* lhs, Expr* rhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(Register::get_register(8));                   
    rhs -> load_register(Register::get_register(9));                   
    mips::binary_div("div", "mfhi", Register::get_register(8)->to_string(), Register::get_register(9)->to_string(),save -> to_string(), prev.c_str());  
}

inline void cmd_assign(Id* lhs, Expr* rhs, const std::string prev = "") {                             
    rhs -> load_register(Register::get_register(8));                   
    mips::access("sw", Register::get_register(8)->to_string(), lhs -> location(), prev.c_str()); 
}

inline void cmd_not(Expr* lhs, Register* save, const std::string prev = "") {   
    lhs -> load_register(save);
    mips::binary_register("sltu", Register::get_register(0)->to_string(), save -> to_string(), Register::get_register(8)->to_string(), prev.c_str()); 
    mips::binary_imm("xor", save->to_string(), 1, prev.c_str());                 
}

inline void cmd_neq(Expr* lhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(save);                   
    mips::binary_register("subu", Register::get_register(0) -> to_string(), save->to_string(), save -> to_string(), prev.c_str());         
}

inline void cmd_neg(Expr* lhs, Register* save, const std::string prev = "") {                             
    lhs -> load_register(save);
    mips::binary_register("nor", save->to_string(), Register::get_register(0) -> to_string(), save -> to_string(), prev.c_str());     
}

inline void cmd_jump(std::string label, const std::string prev = "") {
    mips::jump("b", label);
}

inline void cmd_condition(Expr* lhs, int t, int f, const std::string prev = "", const std::string prev2 = "") {
    if (t != 0 && f != 0) {
        lhs -> load_register(Register::get_register(8));
        mips::conditional_jump(Register::get_register(8)->to_string(), 1, Register::get_register(9) -> to_string(), "L" + std::to_string(t), prev.c_str());
        mips::jump("b", "L" + std::to_string(f), prev2.c_str());
    } else if (t != 0) {
        lhs -> load_register(Register::get_register(8));
        mips::conditional_jump(Register::get_register(8)->to_string(), 1, Register::get_register(9) -> to_string(), "L" + std::to_string(t), prev.c_str());
    } else if (f != 0) {
        lhs -> load_register(Register::get_register(8));
        mips::conditional_jump(Register::get_register(8)->to_string(), 0, Register::get_register(9) -> to_string(), "L" + std::to_string(f), prev.c_str());
    }
}

inline void cmd_load(Id* lhs, Register* reg, const std::string prev = "") {
    mips::access("lw", reg->to_string(), lhs -> location(), prev.c_str());
    // lhs -> store_register(Register::get_register(8));
}

inline void cmd_save(Id* lhs, Register* reg, const std::string prev = "") {
    mips::access("sw", reg->to_string(), lhs -> location(), prev.c_str());
}

inline void cmd_call(Function* lhs, Register* save, const std::string prev = "") {
    mips::jump("jal", lhs -> to_string(), prev.c_str());
    if (save != Register::get_register(2)) mips::access("move", save -> to_string(), Register::get_register(2) -> to_string(), prev.c_str());
}

inline void cmd_store_pre_reg(Register* lhs, Function* rhs, const std::string prev = "") {
    rhs -> used += 4;
    lhs -> offset = rhs -> totsz - rhs -> used;
    mips::access("sw", lhs -> to_string(), lhs -> location_sp(), prev.c_str());
}

inline void cmd_load_pre_reg(Register* lhs, const std::string prev = "") {
    mips::access("lw", lhs -> to_string(), lhs -> location_sp(), prev.c_str());
}

inline void cmd_print_int(Expr* lhs, const std::string prev = "") {
    lhs -> load_register(Register::get_register(4));
    mips::system_print(prev.c_str());
}

inline void cmd_init() {
    printf(".data\nnewline: .asciiz \"\\n\"\n");
    printf(".text\n");
}

inline void cmd_end(const std::string prev = "") {
    printf("\tli $v0, 10\n\tsyscall\t\t# %s\n", prev.c_str());
}