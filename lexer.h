#pragma once
#include <fstream>
#include "symbols.h"
#include <unordered_map>

/*
    The lexer is responsible for breaking the input stream into tokens.
*/

namespace init {
    extern Type* Int = new Type("int", BASIC, 4);
    extern Word* IF = new Word(init::IF, "if");
    extern Word* ELSE = new Word(init::ELSE, "else");
    extern Word* WHILE = new Word(init::WHILE, "while");
    extern Word* BREAK = new Word(init::BREAK, "break");
    extern Word* CONTINUE = new Word(init::CONTINUE, "continue");
    extern Word* FOR = new Word(init::FOR, "for");
}
class Lexer {
public:
    static int lines;
private:
    char peek;
    std::string input;
    int position;
    int len;
    std::unordered_map<std::string, Token*> words;
public:
    Lexer(std::fstream& file);
    Lexer();
    void readch();
    bool readch(char c);
    Token* scan();
};