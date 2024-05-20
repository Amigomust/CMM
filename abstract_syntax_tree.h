#pragma once
#include "lexer.h"
#include "environment.h"

class Parser {
private:
    Token* look;
    Lexer lex;
    Env* top;
    int used;
public:
    void move();
    Parser(Lexer l);
    void error(std::string s);
    void match(int t);
    void program();
    void decl_function();
    Stmt* decl_variable(Type* t);
    Stmt* block();
    Stmt* stmts();
    Stmt* stmt();
    Stmt* assign();
    SeqExpr* decl_args();
    SeqExpr* get_expr();
    Expr* equal();
    Expr* logic_or();
    Expr* logic_and();
    Expr* bit_or();
    Expr* bit_xor();
    Expr* bit_and();
    Expr* logic_equality();
    Expr* logic_ge_le();
    Expr* add_sub();
    Expr* mul_mod_div();
    Expr* factor();
};