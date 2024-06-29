#include "abstract_syntax_tree.h"

Parser::Parser(Lexer* l) {
    lex = l;
    top = new Env(nullptr);
    move();
}

void Parser::move() {
    look = lex -> scan();
}

void Parser::match(int t) {
    if (look->type == t) {
        move();
    } else {
        error("Syntax error");
    }
}

void Parser::error(std::string s) {
    std::cout << "Error in line" << lex -> lines << ": " << s << "\n";
}
void Parser::init() {
    
    Function::PrintInt = new Function(Word::PRINT_INT, Type::Void, nullptr);
    Function::Enclosing = Function::PrintInt;
    Function::PrintInt -> set_args(new SeqExpr(new Arg(Word::TEMP, Type::Int), nullptr));
    top -> put(Word::PRINT_INT, Function::PrintInt);
}

void Parser::program() {
    init();
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
        Function* fun_id = new Function(tok, t, nullptr);
        Function::Enclosing = fun_id;
        top->put(tok, fun_id);
        Env* saved = top;
        top = new Env(saved);
        SeqExpr* fun_args = decl_args();
        match(')');
        fun_id -> set_args(fun_args);
        fun_id -> used = 0;
        fun_id -> emit_label();
        Stmt* s = block();
        int begin = s -> new_label(); int after = s -> new_label();
        s -> emit_label(begin); s -> gen(begin, after); s -> emit_label(after);
        int next = s -> new_label();
        Stmt* normal_return = new Return();
        normal_return -> gen(after, next);
        normal_return -> emit_label(next);
        delete top;
        top = saved;
    }
}

Stmt *Parser::decl_variable(Type* t) {
    if (look -> type == ';') return Stmt::stmt_null;
    else if (look -> type == ',') move();
    Token* name = look;
    match(init::ID);
    Id* id = new Id(name, t);
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
    Id* id = new Arg(tok, t);
    top -> put(tok, id);
    return new SeqExpr(id, decl_args());
}

Stmt* Parser::block() {
    match('{');
    Env* saved = top;
    top = new Env(saved);
    Stmt* s = stmts();
    match('}');
    delete top;
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
        Type* t = (Type*) look;
        move();
        Stmt* temp = decl_variable(t);
        match(';');
        return temp;
    } else if (look -> type == ';') {
        move();
        return Stmt::stmt_null;
    } else if (look -> type == init::IF) {
        match(init::IF); match('(');
        Expr* x = equal();
        match(')');
        Stmt* s1 = block();
        if (look -> type != init::ELSE) {
            return new If(x, s1);
        }
        match(init::ELSE);
        Stmt* s2 = block();
        return new Else(x, s1, s2);
    } else if (look -> type == init::WHILE) {
        match(init::WHILE); match('(');
        Expr* x = equal();
        match(')');
        While* while_node = new While();
        Stmt* temp = Stmt::Enclosing;
        Stmt::Enclosing = while_node;
        Stmt* s = block();
        while_node -> init(x, s);
        Stmt::Enclosing = temp;
        return while_node;
    } else if (look -> type == init::RETURN) {
        match(init::RETURN);
        Expr* x = (look -> type == ';' ? Expr::expr_null : equal());
        match(';');
        return new Return(x);
    } else if (look -> type == init::CONTINUE) {
        match(init::CONTINUE); match(';');
        return new Continue();
    } else if (look -> type == init::BREAK) {
        match(init::BREAK); match(';');
        return new Break();
    } else {
        Stmt* temp = new Calculate(equal());
        match(';');
        return temp;
    }
}

/*
重写赋值表达式
需要修改的地方：
实现一个新的类，继承 Binary 类，重写 gen 方法和 reduce 方法
*/
 
Expr* Parser::equal() {
    Expr* x = logic_or();
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
    Expr* x = unary();
    while (look -> type == '*' || look -> type == '/' || look -> type == '%') {
        Token* tok = look;
        move();
        x = new Binary(tok, x, unary());
    }
    return x;
}

Expr* Parser::unary() {
    if (look -> type == '-') {
        Token* tok = look;
        move();
        return new Unary(tok, unary());
    } else if (look -> type == '!') {
        Token* tok = look;
        move();
        return new Unary(tok, unary());
    } else if (look -> type == '~') {
        Token* tok = look;
        move();
        return new Unary(tok, unary());
    }
    return factor();
}

Expr* Parser::factor() {
    Expr* x = Expr::expr_null;
    if (look -> type == '(') {
        move();
        x = logic_or();
        match(')');
    } else if (look -> type == init::NUM) {
        x = new Constant(look, Type::Int);
        move();
    } else if (look -> type == init::TRUE) {
        x = Constant::True;
        move();
    } else if (look -> type == init::FALSE) {
        x = Constant::False;
        move();
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
    return x;
}

SeqExpr *Parser::get_expr() {
    if (look -> type == ')') {
        return nullptr;
    }
    if (look -> type == ',') move();
    SeqExpr* args = new SeqExpr(equal(), get_expr());
    return args;
}