#include "lexer.h"

/*
    Constructor for the Lexer. 
        - Initializes the input stream and the position.
        - Initializes the key words.
*/
Lexer::Lexer(std::fstream& code) {
    input = ";";
    peek = ' ';
    position = 0;
    lines = 1;
    len = input.size();
}

void Lexer::readch() {
    if (position < len) {
        peek = input[position];
        position++;
    } else {
        peek = '\0';
    }
}

bool Lexer::readch(char c) {
    readch();
    if (peek != c) {
        return false;
    }
    peek = EOF;
    return true;
}

Token Lexer::scan() {
    for (;; readch()) {
        if (peek == ' ' || peek == '\t') continue;
        else if (peek == '\n') lines ++;
        else break;
    }
    
    if (peek == '>') {
        if (readch('=')) return Token(tag::GE, ">=");
        else return Token('>');
    } else if (peek == '<') {
        if (readch('=')) return Token(tag::LE, "<=");
        else return Token('<');
    } else if (peek == '!') {
        if (readch('=')) return Token(tag::NE, "!=");
        else return Token('!');
    } else if (peek == '=') {
        if (readch('=')) return Token(tag::EQ, "==");
        else return Token('=');
    } else if (peek == '&') {
        if (readch('&')) return Token(tag::AND, "&&");
        else return Token('&');
    } else if (peek == '|') {
        if (readch('|')) return Token(tag::OR, "||");
        else return Token('|');
    } 
    if (std::isdigit(peek)) {
        int v = 0;
        do {
            v = 10 * v + (peek - '0');
            readch();
        } while (std::isdigit(peek));
        return Token(tag::NUM, v);
    }

    if (std::isalpha(peek) || peek == '_') {
        std::string id_name = "";
        do {
            id_name += peek;
            readch();
        } while (std::isalnum(peek) || peek == '_');
        return Token(tag::ID, id_name);
    }

    Token Tok(peek);
    peek = ' ';
    return Tok;
}