#include "node.h"
#include "assemble.h"
#include <cstdio>

int Node::labels = 0;
int Temp::num_counts = 0;
int Arg::num_counts = 0;
Stmt* Stmt::stmt_null = new Stmt();
Expr* Expr::expr_null = new Expr(Word::NOTHING, Type::Void);
Stmt* Stmt::Enclosing = Stmt::stmt_null;
Function* Function::Enclosing = nullptr;
Function* Function::PrintInt = nullptr;
Constant* Constant::True = new Constant(new Number(1), Type::Int);
Constant* Constant::False = new Constant(new Number(0), Type::Int);

Node::Node() {
    lexline = Lexer::lines;
}

void Node::emit(std::string s) {
    // std::cout << '\t' << s << '\n';
}

int Node::new_label() {
    label = ++labels;
    return label;
}

void Node::emit_label(int l) {
    std::cout << "L" + std::to_string(l) << ": \n";
}

void Node::error(std::string s) {
    std::cout << "Error in line:" << lexline << ": " << s << "\n";
}

Expr::Expr(Token* tok, Type* p): Node() {
    op = tok;
    type = p;
}

Expr* Expr::gen(const char* from) {
    return this;
}

Expr* Expr::reduce() {
    return this;
}

std::string Expr::to_string() {
    return op -> to_string();
}

void Expr::jumping(int t, int f) {emit_jumps(reduce(), t, f);}

void Expr::emit_jumps(Expr* test, int t, int f) {
    
    if (t != 0 && f != 0) {
        cmd_condition(test, t, f, 
        "if " + test -> to_string() + " goto L" + std::to_string(t), 
        "goto L" + std::to_string(f));
        emit("if " + test -> to_string() + " goto L" + std::to_string(t));
        emit("goto L" + std::to_string(f));
    } else if (t != 0) {
        cmd_condition(test, t, f, "if " + test -> to_string() + " goto L" + std::to_string(t));
        emit("if " + test -> to_string() + " goto L" + std::to_string(t));
    } else if (f != 0) {
        cmd_condition(test, t, f, "iffalse " + test -> to_string() + " goto L" + std::to_string(f));
        emit("iffalse " + test -> to_string() + " goto L" + std::to_string(f));
    }
}


void Expr::load_register(Register* r, const std::string from) {}

Stmt::Stmt() {}

void Stmt::gen(int a, int b) {}

Id::Id(Token* tok, Type* p, bool set): Expr(tok, p){
    ID_type = 0;
    if (set) set_offset();
}

void Id::set_offset() {
    offset = Function::Enclosing -> used + type -> width;
    Function::Enclosing -> used += type -> width;
}

std::string Id::to_string() {
    return op -> to_string() + "{offset = " + std::to_string(offset) + 
    ", width = " + std::to_string(type -> width) + 
    ", type = " + type -> to_string() +'}';
}

std::string Id::location() {
    assert(Function::Enclosing != nullptr);
    return std::to_string(Function::Enclosing -> totsz - offset) + "($fp)";
}

std::string Id::location_sp() {
    return std::to_string(Function::Enclosing -> totsz - offset) + "($sp)";
}

void Id::load_register(Register* r, const std::string from) {
    cmd_load(this, r, from);
}

void Id::store_register(Register* r, const std::string from) {
    cmd_save(this, r, from);
}

Temp::Temp(Type* p): Id(Word::TEMP, p){
    type = p;
    ID_type = 1;
    num = num_counts ++;
}

std::string Temp::to_string() {
    return "temp" + std::to_string(num) + "{offset = " + std::to_string(offset) + 
    ", width = " + std::to_string(type -> width) + 
    ", type = " + type -> to_string() +'}';
}

Arg::Arg(Token* tok, Type* p): Id(tok, p, false){
    ID_type = 2;
    type = p;
    num = num_counts ++;
    function_name = Function::Enclosing;
    assert(function_name);
    set_offset();
}

Arg::Arg(Arg* arg): Id(arg -> op, arg -> type, false) {
    ID_type = 2;
    type = arg -> type;
    num = arg -> num;
    function_name = arg -> function_name;
    offset = Function::Enclosing -> totsz + arg -> offset;
}

void Arg::set_offset() {
    offset = Function::Enclosing -> used;
    Function::Enclosing -> used -= type -> width;
}

std::string Arg::to_string() {
    return function_name -> to_string() + "_arg" + std::to_string(num) + 
    "{offset = " + std::to_string(offset) + 
    ", width = " + std::to_string(type -> width) + 
    ", type = " + type -> to_string() +'}';
}

Function::Function(Token* tok, Type* t, SeqExpr* args): Id(tok, t, false) {
    this -> args = args;
    ID_type = 3;
    used = 0;
    totsz = 128;
}

void Function::emit_label() {
    std::cout << ".globl " + to_string() + "\n";
    std::cout << to_string() + ":\n";
    mips::manage("addiu", -totsz);
    cmd_store_pre_reg(Register::get_register(30), this, "save $fp");
    cmd_store_pre_reg(Register::get_register(31), this, "save $ra");
    mips::access("move", Register::get_register(30) -> to_string(), Register::get_register(29) -> to_string());
}

std::string Function::to_string() {
    std::string res = op -> to_string();
    if (res == "main") return res;
    res += "_";
    if (args != nullptr) {;
        SeqExpr* args_idx = args;
        while (args_idx != nullptr) {
            res += args_idx -> expr1 -> type -> to_string();
            args_idx = args_idx -> expr2;
            if (args_idx != nullptr) res += "_";
        }
    }
    return res;
}

void Function::set_args(SeqExpr* a) {
    args = a;
}

Op::Op(Token* tok, Type* p): Expr(tok, p) {}

Expr* Op::reduce() {
    Expr* x = gen();
    Temp* t = new Temp(type);
    x -> load_register(Register::get_register(8)); // !x -> reg
    t -> store_register(Register::get_register(8), t -> to_string() + " = " + x -> to_string()); // reg -> temp
    emit(t -> to_string() + " = " + x -> to_string());
    return t;
}

Unary::Unary(Token* tok, Expr* expr1): Op(tok, expr1 -> type) {
    expr = expr1;
}

Expr* Unary::gen(const char* from) {
    return new Unary(op, expr -> reduce());
}

std::string Unary::to_string() {
    return op -> to_string() + expr -> to_string();
}

void Unary::load_register(Register* r, const std::string from) {
    if (op -> type == '~') {
        cmd_neg(expr, r, from);
    } else if (op -> type == '!') {
        cmd_not(expr, r, from);
    } else if (op -> type == '-') {
        cmd_neq(expr, r, from);
    }
}

Binary::Binary(Token* tok, Expr* e1, Expr* e2): Op(tok, nullptr) {
    type = Type::max(e1 -> type, e2 -> type);
    if (type == nullptr) error("type error");
    expr1 = e1;
    expr2 = e2;
}

Expr* Binary::gen(const char* from) {
    return new Binary(op, expr1 -> reduce(), expr2 -> reduce());
}

void Binary::load_register(Register* r, const std::string from) {
    if (op -> type == '+') {
        cmd_add(expr1, expr2, r, from);
    } else if (op -> type == '-') {
        cmd_sub(expr1, expr2, r, from);
    } else if (op -> type == '*') {
        cmd_mul(expr1, expr2, r, from);
    } else if (op -> type == '/') {
        cmd_div(expr1, expr2, r, from);
    } else if (op -> type == '%') {
        cmd_mod(expr1, expr2, r, from);
    } else if (op -> type == '<') {
        cmd_slt(expr1, expr2, r, from);
    } else if (op -> type == '>') {
        cmd_sgt(expr1, expr2, r, from);
    } else if (op -> type == '&') {
        cmd_and(expr1, expr2, r, from);
    } else if (op -> type == '|') {
        cmd_or(expr1, expr2, r, from);
    } else if (op -> type == '^') {
        cmd_xor(expr1, expr2, r, from);
    } else if (op -> type == init::EQ) {
        cmd_eq(expr1, expr2, r, from);
    } else if (op -> type == init::NE) {
        cmd_ne(expr1, expr2, r, from);
    } else if (op -> type == init::GE) {
        cmd_ge(expr1, expr2, r, from);
    } else if (op -> type == init::LE) {
        cmd_le(expr1, expr2, r, from);
    } else if (op -> type == init::AND) {
        cmd_logic_and(expr1, expr2, r, from);
    } else if (op -> type == init::OR) {
        cmd_logic_or(expr1, expr2, r, from);
    } else if (op -> type == '=') {
        assert(dynamic_cast<Id*>(expr1) != nullptr);
        cmd_assign((Id*)expr1, expr2, from);
    } else {
        debug(op -> to_string());
        assert(false);
    }
}

std::string Binary::to_string() {
    return expr1 -> to_string() + " " + op -> to_string() + " " + expr2 -> to_string();
}

Call::Call(Token* tok, Type* t, Function* f, SeqExpr* a): Expr(tok, t) {
    function = f;
    args = a;
    fun_args = f -> args;
}

Expr* Call::gen(const char* from) {
    SeqExpr* args_idx = this -> args;
    SeqExpr* fun_args_idx = this -> fun_args;
    Arg* argid = nullptr;
    while (args_idx != nullptr && fun_args_idx != nullptr) {
        argid = new Arg((Arg*) fun_args_idx -> expr1);
        Set* set = new Set(argid, args_idx -> expr1);
        set -> gen(0, 0);
        args_idx = args_idx -> expr2;
        fun_args_idx = fun_args_idx -> expr2;
    }
    if (function == Function::PrintInt) {
        assert(argid != nullptr);
        cmd_print_int(argid, "print_int" + argid -> to_string());
    }
    return this;
}

Expr* Call::reduce() {
    Expr* x = gen();
    Temp* t = new Temp(type);
    x -> load_register(Register::get_register(8));
    t -> store_register(Register::get_register(8), t -> to_string() + " = " + x -> to_string());
    emit(t -> to_string() + " = " + x -> to_string());
    return t;
}

std::string Call::to_string() {
    return "call " + function -> to_string();
}

void Call::load_register(Register* r, const std::string from) {
    cmd_call(function, r, from);
}

Constant::Constant(Token* tok, Type* p): Expr(tok, p) {}

void Constant::jumping(int t, int f) {
    if (op -> value == 0 && f != 0) {
        cmd_jump("L" + std::to_string(f));
        emit("goto L" + std::to_string(f));
    }
    else if (t != 0 && t != 0) {
        cmd_jump("L" + std::to_string(t));
        emit("goto L" + std::to_string(t));
    }
}

std::string Constant::to_string() {
    return std::to_string(op -> value);
}

void Constant::load_register(Register* r, const std::string from) {
    // TODO -> 重写
    mips::binary_imm("li", r -> to_string(), op -> value, from.c_str());
}

Set::Set(Id* i, Expr* x) {
    id = i;
    expr = x;
    if (check(id -> type, expr -> type) == nullptr) error("type error");
}

Type* Set::check(Type* p1, Type* p2) {
    if (Type::numeric(p1) && Type::numeric(p2)) return Type::max(p1, p2);
    else return nullptr;
}

void Set::gen(int b, int a) {
    Expr* x = expr -> gen();
    x -> load_register(Register::get_register(8));
    id -> store_register(Register::get_register(8), id -> to_string() + " = " + x -> to_string());
    emit(id -> to_string() + " = " + x -> to_string());
}

Calculate::Calculate(Expr* x) {
    expr = x;
}

void Calculate::gen(int b, int a) {
    Expr* x = expr -> gen();
    if (x -> type != Type::Void) {
        x -> load_register(Register::get_register(8), x -> to_string());
    }
    emit(x -> to_string());
}

std::string Calculate::to_string() {
    return expr -> to_string();
}

Decl::Decl(Type* t, Id* i, Stmt* x) {
    type = t;
    id = i;
    expr = x;
}

void Decl::gen(int b, int a) {
    emit(type -> to_string() + " " + id -> to_string());
    expr -> gen(b, a);
}

SeqStmt::SeqStmt(Stmt* s1, Stmt* s2): Stmt() {
    stmt1 = s1;
    stmt2 = s2;
}

void SeqStmt::gen(int a, int b) {
    if (stmt1 == Stmt::stmt_null) {
        stmt2 -> gen(a, b);
    } else if (stmt2 == Stmt::stmt_null) {
        stmt1 -> gen(a, b);
    } else {
        new_label();
        stmt1 -> gen(a, label);
        emit_label(label);
        stmt2 -> gen(label, b);
    }
}

SeqExpr::SeqExpr(Expr* e1, SeqExpr* e2): Expr(nullptr, nullptr) {
    number = (e2 == nullptr ? 0 : e2 -> number) + 1;
    expr1 = e1;
    expr2 = e2;
}

If::If(Expr* x, Stmt* s) {
    expr = x;
    stmt = s;
    // if (expr -> type != Type::Bool) error("boolean required in if");
}

void If::gen(int a, int b) {
    int label = new_label();
    expr -> jumping(0, b);
    emit_label(label); stmt -> gen(label, b);
}

Else::Else(Expr* x, Stmt* s1, Stmt* s2) {
    expr = x;
    stmt1 = s1;
    stmt2 = s2;
    // if (expr -> type != Type::Bool) error("boolean required in if");
}

void Else::gen(int a, int b) {
    int label1 = new_label();
    int label2 = new_label();
    expr -> jumping(0, label2);
    emit_label(label1); stmt1 -> gen(label1, b); cmd_jump("L" + std::to_string(b), "goto L" + std::to_string(b)); emit("goto L" + std::to_string(b));
    emit_label(label2); stmt2 -> gen(label2, b);
}

While::While(Expr* x, Stmt* s) {
    expr = x;
    stmt = s;
}

While::While() {
    expr = Expr::expr_null;
    stmt = Stmt::stmt_null;
}

void While::init(Expr* x, Stmt* s) {
    expr = x;
    stmt = s;
}

void While::gen(int a, int b) {
    after = b;
    begin = a;
    expr -> jumping(0, b);
    int label = new_label();
    emit_label(label); stmt -> gen(label, a);
    cmd_jump("L" + std::to_string(a), "goto L" + std::to_string(a));
    emit("goto L" + std::to_string(a));
}

Continue::Continue() {
    if (Stmt::Enclosing == Stmt::stmt_null) error("unenclosed continue");
    stmt = Stmt::Enclosing;
}

void Continue::gen(int a, int b) {
    cmd_jump("L" + std::to_string(stmt -> begin), "goto L" + std::to_string(stmt -> begin));
    emit("goto L" + std::to_string(stmt -> begin));
}

Break::Break() {
    if (Stmt::Enclosing == Stmt::stmt_null) error("unenclosed break");
    stmt = Stmt::Enclosing;
}

void Break::gen(int a, int b) {
    cmd_jump("L" + std::to_string(stmt -> after), "goto L" + std::to_string(stmt->after));
    emit("goto L" + std::to_string(stmt->after));
}

Return::Return(Expr* x) {
    expr = x;
    if (Function::Enclosing == nullptr) error("unenclosed return");
    func = Function::Enclosing;
    if (expr -> type != func -> type) error("return type error");
}

Return::Return() {
    expr = Expr::expr_null;
    if (Function::Enclosing == nullptr) error("?unenclosed return?");
    func = Function::Enclosing;
}

void Return::gen(int a, int b) {
    Expr* x = expr -> gen(__PRETTY_FUNCTION__);
    x -> load_register(Register::get_register(2));
    cmd_load_pre_reg(Register::get_register(30), "load $fp");
    cmd_load_pre_reg(Register::get_register(31), "load $ra");
    mips::manage("addiu", func -> totsz);
    if (func -> op -> to_string() == "main") {
        cmd_end("end main");
    } else {
        mips::return_value("", ("return " + x -> to_string()).c_str());
        emit("return " + x -> to_string());
    }
}