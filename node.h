#pragma once
#include "register.h"
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
    virtual Expr* gen(const char* from = "");
    virtual Expr* reduce();
    virtual std::string to_string();
    virtual void jumping(int t, int f);
    virtual void emit_jumps(Expr* test, int t, int f);
    virtual void load_register(Register* r, const std::string from = "");
    
};

class Id : public Expr {
protected:
    int offset;
public:
    int ID_type;
    Id(Token* tok, Type* p, bool set = true);
    virtual void set_offset();
    std::string to_string();
    virtual std::string location();
    virtual std::string location_sp();
    void load_register(Register* r, const std::string from ="");
    virtual void store_register(Register* r, const std::string from = "");
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
    Id* function_name;
public:
    Arg(Token* tok, Type* p);
    Arg(Arg* a);
    std::string to_string();
    void set_offset();
};

class SeqExpr: public Expr {
public:
    int number;
    Expr* expr1;
    SeqExpr* expr2;
    SeqExpr(Expr* e1, SeqExpr* e2);
};

class Function: public Id {
private:
    SeqExpr* args;
public:
    static Function* Enclosing;
    static Function* PrintInt;
    int used;
    int totsz;
    Function(Token* tok, Type* t, SeqExpr* args);
    void emit_label();
    friend class Call;
    std::string to_string();
    void set_args(SeqExpr* a);
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
    Expr* gen(const char* from = "");
    std::string to_string();
    void load_register(Register* r, const std::string from = "");
};

class Binary: public Op {
private:
    Expr* expr1;
    Expr* expr2;
public:
    Binary(Token* tok, Expr* expr1, Expr* expr2);
    Expr* gen(const char* from = "");
    std::string to_string();
    void load_register(Register* r, const std::string from = "");
};

class Call: public Expr {
private:
    Function* function;
    SeqExpr* args;
    SeqExpr* fun_args;
public:
    Call(Token* tok, Type* t, Function* f, SeqExpr* a);
    Expr* gen(const char* from = "");
    Expr* reduce();
    std::string to_string();
    void load_register(Register* r, const std::string from = "");
};

class Constant: public Expr {
public:
    static Constant* True;
    static Constant* False;
    Constant(Token* tok, Type* p);
    void jumping(int t, int f);
    std::string to_string();
    void load_register(Register* r, const std::string from = "");
};

class Stmt : public Node {
private:
public:
    static Stmt* stmt_null;
    static Stmt* Enclosing;
    int after;
    int begin;
    Stmt();
    virtual void gen(int a, int b);
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

class Calculate: public Stmt {
private:
    Expr* expr;
public:
    Calculate(Expr* x);
    void gen(int b, int a);
    std::string to_string();
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

class If: public Stmt {
private:
    Expr* expr;
    Stmt* stmt;
public:
    If(Expr* x, Stmt* s);
    void gen(int a, int b);
    std::string to_string();
};

class Else: public Stmt {
private:
    Expr* expr;
    Stmt* stmt1;
    Stmt* stmt2;
public:
    Else(Expr* x, Stmt* s1, Stmt* s2);
    void gen(int a, int b);
};

class While: public Stmt {
private:
    Expr* expr;
    Stmt* stmt;
public: 
    While(Expr* x, Stmt* s);
    While();
    void init(Expr* x, Stmt* s);
    void gen(int a, int b);
};

class Continue: public Stmt {
private:
    Stmt* stmt;
public:
    Continue();
    void gen(int a, int b);
};

class Break: public Stmt {
private:
    Stmt* stmt;
public:
    Break();
    void gen(int a, int b);
};

class Return: public Stmt {
private:
    Function* func;
    Expr* expr;
public:
    Return(Expr* x);
    Return();
    void gen(int a, int b);
};