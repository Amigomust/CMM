#pragma once
#include "lexer.h"

class Node {
public:
    static int labels;
    int label;
    int lexline;
    Node();
    int new_label();
    void emit_label(int l);
protected:
    void emit(std::string s);
    void error(std::string s);
};

class Expr : public Node {
public:
    static Expr* expr_null;
    Token* op;
    Type* type;
    Expr(Token* tok, Type* p);
    virtual Expr* gen();
    virtual Expr* reduce();
    virtual std::string to_string();
};

class Id : public Expr {
private:
    int offset;
    
public:
    int ID_type;
    Id(Token* tok, Type* p, int b);
};

class Temp: public Id {
private:
    static int num_counts;
    int num;
public:
    Temp(Type* p);
    std::string to_string();
};

class Arg: public Id {
private:
    static int num_counts;
    int num;
public:
    Arg(Type* p);
    std::string to_string();
};

class Function: public Id {
private:
    SeqExpr* args;
public:
    Function(Token* tok, Type* t, SeqExpr* args);
    friend class Call;
};

class Op: public Expr {
public:
    Op(Token* tok, Type* p);
    Expr* reduce();
};

class Unary: public Op {
private:
    Expr* expr;
public:
    Unary(Token* tok, Expr* expr);
    Expr* gen();
    std::string to_string();
};

class Binary: public Op {
private:
    Expr* expr1;
    Expr* expr2;
public:
    Binary(Token* tok, Expr* expr1, Expr* expr2);
    Expr* gen();
    std::string to_string();
};

class Call: public Expr {
private:
    Function* function;
    SeqExpr* args;
    SeqExpr* fun_args;
public:
    Call(Token* tok, Type* t, Function* f, SeqExpr* a);
    Expr* gen();
    Expr* reduce();
};

class Constant: public Expr {
public:
    Constant(Token* tok, Type* p);
};

class Stmt : public Node {
private:
public:
    static Stmt* stmt_null;
    Stmt();
    void gen(int a, int b);
};

class Set: public Stmt {
private:
    Id* id;
    Expr* expr;
public:
    Set(Id* i, Expr* x);
    void gen(int b, int a);
    Type* check(Type* p1, Type* p2);
};

class Decl: public Stmt {
private:
    Type* type;
    Id* id;
    Stmt* expr;
public:
    Decl(Type* t, Id* i, Stmt* x);
    void gen(int b, int a);
};

class SeqStmt: public Stmt {
private:
    Stmt* stmt1;
    Stmt* stmt2;
public:
    SeqStmt(Stmt* s1, Stmt* s2);
    void gen(int a, int b);
};

class SeqExpr: public Expr {
public:
    Expr* expr1;
    SeqExpr* expr2;
    SeqExpr(Expr* e1, SeqExpr* e2);
};