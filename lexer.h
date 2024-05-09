#pragma once
#include <fstream>
#include <symbols.h>
#include <unordered_map>

/*
    The lexer is responsible for breaking the input stream into tokens.
*/
class Lexer { 
private:
    char peek;
    std::string input;
    int position;
    int lines;
    int len;
    std::unordered_map<std::string, Token> words;
public:
    Lexer(std::fstream& file);
    void readch();
    bool readch(char c);
    Token scan();
};