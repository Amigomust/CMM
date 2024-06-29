#pragma once
#include <iostream>
#include <assert.h>

#ifdef LOCAL
#include "debug.h"
#else
#define debug(...) do {} while (0)
#define pause(...) do {} while (0)
#endif

namespace init {
    const int AND = 256, TRUE = 257, FALSE = 258, NOT = 259, OR = 260,
        NUM = 261, ID = 262, IF = 263, ELSE = 264, WHILE = 265,
        BREAK = 266, CONTINUE = 267, FOR = 268, DO = 269,
        NE = 270, EQ = 271, LE = 272, GE = 273, BASIC = 274, RETURN = 275, END = 277;
}
// A token is a single unit of input. It is a pair of a type and a value.
class Token {
/*
    Type:
        - Token
        - Number
        - Word
            - Type
            - Array     TODO
*/
public:
    int type;
    int value;
    std::string lexme;
    size_t width;
    Token(int t);
    virtual std::string to_string();
};

class Word : public Token {
public:
    std::string lexme;
    // Constructor for a token with only a token_type.
    Word(int t, std::string s);
    std::string to_string();
    static Word* IF, *ELSE, *WHILE, *BREAK, *CONTINUE, *FOR, *TEMP, *NOTHING, *PRINT_INT;
};

class Number : public Token {
public:
    // Constructor for a token Number(value).
    Number(int v);
    std::string to_string();
};

class Type : public Word {
public:
    static Type* Int;
    static Type* Bool;
    static Type* Void;
    static Type* Float;
    size_t width = 0;
    // Constructor for a token Type(token_type, typename, width).
    Type(std::string s, int t, size_t w);
    static bool numeric(Type* p);
    static Type* max(Type* p1, Type* p2);
};
