#include "abstract_syntax_tree.h"

Parser::Parser(Lexer l) {
    lex = l;
    move();
}

void Parser::move() {
    look = lex.scan();
}

void Parser::match(int t) {
    if (look->type == t) {
        move();
    } else {
        error("Syntax error");
    }
}

void Parser::error(std::string s) {
    std::cout << "Error in line" << lex.lines << ": " << s << "\n";
}

void Parser::program() {
    if (look->type == init::END) {
        return;
    }
    decl_function();
}

void Parser::decl_function() {
    while (look -> type == init::BASIC) {
        Type* t = (Type*) look;
        match(init::BASIC);
        Token* tok = look;
        match(init::ID);
        match('(');
        Env* saved = top;
        top = new Env(saved);
        SeqExpr* fun_args = decl_args();
        match(')');
        top->put(tok, new Function(tok, t, fun_args));
        Stmt* s = block();
        int begin = s -> new_label(); int after = s -> new_label();
        s -> emit_label(begin); s -> gen(begin, after); s -> emit_label(after);
    }
}

Stmt *Parser::decl_variable(Type* t) {
    if (look -> type == ';') return Stmt::stmt_null;
    else if (look -> type == ',') move();
    Token* name = look;
    match(init::ID);
    Id* id = new Id(name, t, used);
    top -> put(name, id);
    Stmt* s = Stmt::stmt_null;
    if (look -> type == '=') {
        move();
        s = new Set(id, equal());
    }
    return new SeqStmt(new Decl(t, id, s), decl_variable(t));
}

SeqExpr *Parser::decl_args() {
    if (look -> type == ')') return nullptr;
    if (look -> type == ',') move();
    Type* t = (Type*) look;
    match(init::BASIC);
    Token* tok = look;
    match(init::ID);
    Id* id = new Id(tok, t, used);
    top -> put(tok, id);
    return new SeqExpr(id, decl_args());
}

Stmt* Parser::block() {
    match('{');
    Env* saved = top;
    top = new Env(saved);
    Stmt* s = stmts();
    match('}');
    top = saved;
    return s;
}

Stmt* Parser::stmts() {
    if (look -> type == '}') {
        return Stmt::stmt_null;
    } else {
        return new SeqStmt(stmt(), stmts());
    }
}

Stmt* Parser::stmt() {
    if (look -> type == init::BASIC) {
        move();
        return decl_variable((Type*)look);
    } else if (look -> type == ';') {
        move();
        return Stmt::stmt_null;
    } else {
        return assign();
    }
}

Stmt* Parser::assign() {
    Stmt* stmt;
    Token* t = look;
    match(init::ID);
    Id* id = top->get(t);
    if (id == nullptr) {
        error(id->to_string() + " undeclared");
    }
    if (look -> type == '=') {
        move();
        stmt = new Set(id, equal());
        return;
    } else {
        equal();
    }
    match(';');
    return stmt;
}

Expr* Parser::equal() {
    Expr* x = logic_or();
    if (x -> op -> type != init::ID) {
        error("Error in equal left value");
    }
    while (look -> type == '=') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, logic_or());
    }
    return x;
}

Expr* Parser::logic_or() {
    Expr* x = logic_and();
    while (look -> type == init::OR) {
        Token* tok = look;
        move();
        x = new Binary                                                                                                                                                                                                                                                             (tok, x, logic_and());
    }
    return x;
}

Expr* Parser::logic_and() {
    Expr* x = bit_or();
    while (look -> type == init::AND) {
        Token* tok = look;
        move();
        x = new Binary(tok, x, bit_or());
    }
    return x;
}

Expr* Parser::bit_or() {
    Expr* x = bit_xor();
    while (look -> type == '|') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, bit_xor());
    }
    return x;
}

Expr* Parser::bit_xor() {
    Expr* x = bit_and();
    while (look -> type == '^') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, bit_and());
    }
    return x;
}

Expr* Parser::bit_and() {
    Expr* x = logic_equality();
    while (look -> type == '&') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, logic_equality());
    }
    return x;
}

Expr* Parser::logic_equality() {
    Expr* x = logic_ge_le();
    while (look -> type == init::EQ || look -> type == init::NE) {
        Token* tok = look;
        move();
        x = new Binary(tok, x, logic_ge_le());
    }
    return x;
}

Expr* Parser::logic_ge_le() {
    Expr* x = add_sub();
    while (look -> type == init::LE || look -> type == '<' || 
    look -> type == init::GE || look -> type == '>') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, add_sub());
    }
    return x;
}

Expr* Parser::add_sub() {
    Expr* x = mul_mod_div();
    while (look -> type == '+' || look -> type == '-') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, mul_mod_div());
    }
    return x;
}

Expr* Parser::mul_mod_div() {
    Expr* x = factor();
    while (look -> type == '*' || look -> type == '/' || look -> type == '%') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, factor());
    }
}

Expr* Parser::factor() {
    Expr* x = nullptr;
    if (look -> type == '(') {
        move();
        x = equal();
        match(')');
    } else if (look -> type == init::NUM) {
        x = new Constant(look, Type::Int);
    } else if (look -> type == init::ID) {
        Id* id = top->get(look);
        if (id == nullptr) {
            error(look -> to_string() + " undeclared");
        }
        move();
        if (look -> type != '(') {
            if (id -> ID_type == 3) error(id -> to_string() + " is not a variable");
            return id;
        }
        match('(');
        if (id -> ID_type != 3) error(id -> to_string() + " is not a function");
        Function* fun_id = (Function*) id;
        SeqExpr* args = get_expr();
        match(')');   
        x = new Call(look, id -> type, fun_id, args);
    } else {
        error("Syntax error");
    }
}


SeqExpr *Parser::get_expr() {
    if (look -> type == ')') {
        return nullptr;
    }
    if (look -> type == ',') move();
    SeqExpr* args = new SeqExpr(equal(), get_expr());
    return args;
}