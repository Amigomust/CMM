#include "node.h"

int Node::labels = 0;
int Temp::num_counts = 0;
int Arg::num_counts = 0;
Stmt* Stmt::stmt_null = new Stmt();

Node::Node() {
    lexline = Lexer::lines;
}

void Node::emit(std::string s) {
    std::cout << s << '\n';
}

int Node::new_label() {
    label = ++labels;
    return label;
}

void Node::emit_label(int l) {
    std::cout << "L" + std::to_string(l) << ": ";
}

void Node::error(std::string s) {
    std::cout << "Error in line" << lexline << ": " << s << "\n";

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

Stmt::Stmt() {}

void Stmt::gen(int a, int b) {}

Id::Id(Token* tok, Type* p, int b): Expr(tok, p){
    offset = b;
    ID_type = 0;
}

Temp::Temp(Type* p): Id(nullptr, p, 0){
    type = p;
    ID_type = 1;
    num = num_counts ++;
}

std::string Temp::to_string() {
    return "t" + std::to_string(num);
}

Arg::Arg(Type* p): Id(nullptr, p, 0){
    ID_type = 2;
    type = p;
    num = num_counts ++;
}

std::string Arg::to_string() {
    return "arg" + std::to_string(num);
}

Function::Function(Token* tok, Type* t, SeqExpr* args): Id(tok, t, 0) {
    this -> args = args;
    ID_type = 3;
    emit("." + tok -> to_string() + ":");
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

Binary::Binary(Token* tok, Expr* expr1, Expr* expr2): Op(tok, nullptr) {
    type = Type::max(expr1 -> type, expr2 -> type);
    if (type == nullptr) error("type error");
    expr1 = expr1 -> reduce();
    expr2 = expr2 -> reduce();
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

Constant::Constant(Token* tok, Type* p): Expr(tok, p) {}

Expr* Call::reduce() {
    Expr* x = gen();
    Temp* t = new Temp(type);
    emit(t -> to_string() + " = call " + op -> to_string());
    return t;
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
    expr1 = e1;
    expr2 = e2;
}
