#include "lexer.h"

/*
    Constructor for the Lexer. 
        - Initializes the input stream and the position.
        - Initializes the key words.
*/

int Lexer::lines = 0;
Lexer::Lexer(std::fstream& code) {
    input = ";";
    peek = ' ';
    position = 0;
    len = input.size();
}

void Lexer::readch() {
    if (position < len) {
        peek = input[position];
        position++;
    } else {
        peek = init::END;
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

Token* Lexer::scan() {
    for (;; readch()) {
        if (peek == ' ' || peek == '\t') continue;
        else if (peek == '\n') lines ++;
        else break;
    }
    
    if (peek == '>') {
        if (readch('=')) return new Word(init::GE, ">=");
        else return new Token('>');
    } else if (peek == '<') {
        if (readch('=')) return new Word(init::LE, "<=");
        else return new Token('<');
    } else if (peek == '!') {
        if (readch('=')) return new Word(init::NE, "!=");
        else return new Token('!');
    } else if (peek == '=') {
        if (readch('=')) return new Word(init::EQ, "==");
        else return new Token('=');
    } else if (peek == '&') {
        if (readch('&')) return new Word(init::AND, "&&");
        else return new Token('&');
    } else if (peek == '|') {
        if (readch('|')) return new Word(init::OR, "||");
        else return new Token('|');
    } 
    if (std::isdigit(peek)) {
        int v = 0;
        do {
            v = 10 * v + (peek - '0');
            readch();
        } while (std::isdigit(peek));
        return new Number(v);
    }

    if (std::isalpha(peek) || peek == '_') {
        std::string id_name = "";
        do {
            id_name += peek;
            readch();
        } while (std::isalnum(peek) || peek == '_');
        if (words.find(id_name) == words.end()) {
            words[id_name] = new Word(init::ID, id_name);
        }
        return words[id_name];
    }

    Token* Tok = new Token(peek);
    peek = ' ';
    return Tok;
}