#include "symbols.h"

/*
    Constructor for a token with only a token_type.
*/
Token::Token(int t) {
    type = t;
}

/*
    Constructor for a token Word(token_type, lexme).
*/
Token::Token(int t, std::string l) {
    type = t;
    lexme = l;
}

/*
    Constructor for a token Number(token_type, value).
*/
Token::Token(int t, int x) {

    type = t;
    value = x;
}

/*
    Constructor for a token Type(token_type, typename, width).
*/
Token::Token(int t, std::string type, size_t w) {
    type = t;
    lexme = type;
    width = w;
}


