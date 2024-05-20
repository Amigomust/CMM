#include "symbols.h"


namespace init {
    const int AND = 256, TRUE = 257, FALSE = 258, NOT = 259, OR = 260,
        NUM = 261, ID = 262, IF = 263, ELSE = 264, WHILE = 265,
        BREAK = 266, CONTINUE = 267, FOR = 268, DO = 269,
        NE = 270, EQ = 271, LE = 272, GE = 273, BASIC = 274, END = 275;
    
}

Type* Type::Int = new Type("int", init::BASIC, 4);

/*
    Constructor for a token with only a token_type.
*/
Token::Token(int t) {
    type = t;
}


std::string Token::to_string() {
    return "" + char(type);
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