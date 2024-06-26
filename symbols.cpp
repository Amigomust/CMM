#include "symbols.h"

Type* Type::Int = new Type("int", init::BASIC, 4);
Type* Type::Bool = new Type("bool", init::BASIC, 1);
Type* Type::Void = new Type("void", init::BASIC, 0);
Type* Type::Float = new Type("float", init::BASIC, 4);
Word* Word::IF = new Word(init::IF, "if");
Word* Word::ELSE = new Word(init::ELSE, "else");
Word* Word::WHILE = new Word(init::WHILE, "while");
Word* Word::BREAK = new Word(init::BREAK, "break");
Word* Word::CONTINUE = new Word(init::CONTINUE, "continue");
Word* Word::FOR = new Word(init::FOR, "for");

/*
    Constructor for a token with only a token_type.
*/
Token::Token(int t) {
    type = t;
}


std::string Token::to_string() {
    std::string s = "";
    return s + char(type);
}

/*
    Constructor for a token Word(token_type, lexme).
*/
Word::Word(int t, std::string l): Token(t){
    lexme = l;
}

std::string Word::to_string() {
    return lexme;
}

/*
    Constructor for a token Number(token_type, value).
*/
Number::Number(int x): Token(init::NUM){
    value = x;
}

std::string Number::to_string() {
    return std::to_string(value);
}

/*
    Constructor for a token Type(token_type, typename, width).
*/
Type::Type(std::string type, int t, size_t w): Word(t, type){
    width = w;
}

bool Type::numeric(Type* p) {
    if (p == Type::Int) return true;
    else return false;
}

Type* Type::max(Type* p1, Type* p2) {
    if (!numeric(p1) || !numeric(p2)) return nullptr;
    else if (p1 == Type::Int) return p1;
    else return p2;
}