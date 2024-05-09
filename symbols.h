
#include <iostream>

namespace tag {
    const int AND = 256, TRUE = 257, FALSE = 258, NOT = 259, OR = 260,
        NUM = 261, ID = 262, IF = 263, ELSE = 264, WHILE = 265,
        BREAK = 266, CONTINUE = 267, FOR = 268, DO = 269,
        NE = 270, EQ = 271, LE = 272, GE = 273, BASIC = 274;
}

class Token {
/*
    A token is a single unit of input. It is a pair of a type and a value.
    Type:
        - Token
        - Number
        - Identifier
        - Type
*/
public:
    int type;
    int value;
    std::string lexme;
    size_t width;
    Token(int t);
    Token(int t, int v);
    Token(int t, std::string l);
    Token(int t, std::string type, size_t w);
};