#include "node.h"
#include <cstdio>

int Node::labels = 0;
int Temp::num_counts = 0;
int Arg::num_counts = 0;
Stmt* Stmt::stmt_null = new Stmt();
Expr* Expr::expr_null = new Expr(nullptr, Type::Void);
Stmt* Stmt::Enclosing = Stmt::stmt_null;
Function* Function::Enclosing = nullptr;
Constant* Constant::True = new Constant(new Number(100), Type::Int);
Constant* Constant::False = new Constant(new Number(0), Type::Int);

Node::Node() {
    lexline = Lexer::lines;
}

void Node::emit(std::string s) {
    std::cout << '\t' << s << '\n';
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

Expr* Expr::gen() {
    return this;
}

Expr* Expr::reduce() {
    return this;
}

std::string Expr::to_string() {
    return op -> to_string();
}

void Expr::jumping(int t, int f) {emit_jumps(to_string(), t, f);}

void Expr::emit_jumps(std::string test, int t, int f) {
    if (t != 0 && f != 0) {
        emit("if " + test + " goto L" + std::to_string(t));
        emit("goto L" + std::to_string(f));
    } else if (t != 0) {
        emit("if " + test + " goto L" + std::to_string(t));
    } else if (f != 0) {
        emit("iffalse " + test + " goto L" + std::to_string(f));
    }
}

Stmt::Stmt() {}

void Stmt::gen(int a, int b) {}

Id::Id(Token* tok, Type* p, bool set): Expr(tok, p){
    ID_type = 0;
    if (set) set_offset();
}

void Id::set_offset() {
    offset = Function::Enclosing -> used;
    Function::Enclosing -> used += type -> width;
}

std::string Id::to_string() {
    return op -> to_string() + "{offset = " + std::to_string(offset) + 
    ", width = " + std::to_string(type -> width) + 
    ", type = " + type -> to_string() +'}';
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

void Arg::set_offset() {
    Function::Enclosing -> used -= type -> width;
    offset = Function::Enclosing -> used;
}

std::string Arg::to_string() {
    return function_name -> to_string() + "_arg" + std::to_string(num) + 
    "{offset = " + std::to_string(offset) + 
    ", width = " + std::to_string(type -> width) + 
    ", type = " + type -> to_string() +'}';
}

Function::Function(Token* tok, Type* t, SeqExpr* args): Id(tok, t, false) {;
    this -> args = args;
    ID_type = 3;
    emit_label();
    used = 0;
}

void Function::emit_label() {
    std::cout << to_string() + ":\n";
}

std::string Function::to_string() {
    std::string res = '<' + type -> to_string() + '>' + op -> to_string();
    res += "(";
    if (args != nullptr) {
        SeqExpr* args_idx = args;
        while (args_idx != nullptr) {
            res += args_idx -> expr1 -> type -> to_string();
            args_idx = args_idx -> expr2;
            if (args_idx != nullptr) res += ", ";
        }
    }
    res += ")";
    return res;
}

void Function::set_args(SeqExpr* a) {
    args = a;
}

Op::Op(Token* tok, Type* p): Expr(tok, p) {}

Expr* Op::reduce() {
    Expr* x = gen();
    Temp* t = new Temp(type);
    emit(t -> to_string() + " = " + x -> to_string());
    return t;
}

Unary::Unary(Token* tok, Expr* expr1): Op(tok, expr1 -> type) {
    expr = expr1;
}

Expr* Unary::gen() {
    return new Unary(op, expr -> reduce());
}

std::string Unary::to_string() {
    return op -> to_string() + expr -> to_string();
}

Binary::Binary(Token* tok, Expr* e1, Expr* e2): Op(tok, nullptr) {
    type = Type::max(e1 -> type, e2 -> type);
    if (type == nullptr) error("type error");
    expr1 = e1;
    expr2 = e2;
}

Expr* Binary::gen() {
    return new Binary(op, expr1 -> reduce(), expr2 -> reduce());
}

std::string Binary::to_string() {
    return expr1 -> to_string() + " " + op -> to_string() + " " + expr2 -> to_string();
}

Call::Call(Token* tok, Type* t, Function* f, SeqExpr* a): Expr(tok, t) {
    function = f;
    args = a;
    fun_args = f -> args;
}

Expr* Call::gen() {
    SeqExpr* args_idx = this -> args;
    SeqExpr* fun_args_idx = this -> fun_args;
    while (args_idx != nullptr && fun_args_idx != nullptr) {
        Arg* argid = (Arg*) fun_args_idx -> expr1;
        Set* set = new Set(argid, args_idx -> expr1);
        set -> gen(0, 0);
        args_idx = args_idx -> expr2;
        fun_args_idx = fun_args_idx -> expr2;
    }
    return this;
}

Expr* Call::reduce() {
    Temp* t = new Temp(type);
    emit(t -> to_string() + " = " + gen() -> to_string());
    return t;
}

std::string Call::to_string() {
    return "call " + function -> to_string();
}

Constant::Constant(Token* tok, Type* p): Expr(tok, p) {}

void Constant::jumping(int t, int f) {
    if (op -> value == 0 && f != 0) emit("goto L" + std::to_string(f));
    else if (t != 0 && t != 0) emit("goto L" + std::to_string(t));
}

std::string Constant::to_string() {
    return std::to_string(op -> value);
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
    emit(id -> to_string() + " = " + expr -> gen() -> to_string());
}

Calculate::Calculate(Expr* x) {
    expr = x;
}

void Calculate::gen(int b, int a) {
    emit(expr -> gen() -> to_string());
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
    emit_label(label1); stmt1 -> gen(label1, b); emit("goto L" + std::to_string(b));
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
    emit("goto L" + std::to_string(a));
}

Continue::Continue() {
    if (Stmt::Enclosing == Stmt::stmt_null) error("unenclosed continue");
    stmt = Stmt::Enclosing;
}

void Continue::gen(int a, int b) {
    emit("goto L" + std::to_string(stmt -> begin));
}

Break::Break() {
    if (Stmt::Enclosing == Stmt::stmt_null) error("unenclosed break");
    stmt = Stmt::Enclosing;
}

void Break::gen(int a, int b) {
    emit("goto L" + std::to_string(stmt->after));
}

Return::Return(Expr* x) {
    expr = x;
    if (Function::Enclosing == nullptr) error("unenclosed return");
    func = Function::Enclosing;
    if (expr -> type != func -> type) error("return type error");
}

void Return::gen(int a, int b) {
    emit("return " + expr -> gen() -> to_string());
}