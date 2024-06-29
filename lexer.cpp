#include "lexer.h"

/*
    Constructor for the Lexer. 
        - Initializes the input stream and the position.
        - Initializes the key words.
*/

int Lexer::lines = 1;
Lexer::Lexer(std::string code) {
    input = code;
    peek = ' ';
    position = 0;
    len = input.size();
    words["if"] = new Word(init::IF, "if");
    words["else"] = new Word(init::ELSE, "else");
    words["while"] = new Word(init::WHILE, "while");
    words["break"] = new Word(init::BREAK, "break");
    words["continue"] = new Word(init::CONTINUE, "continue");
    words["int"] = Type::Int;
    words["bool"] = Type::Bool;
    words["void"] = Type::Void;
    words["float"] = Type::Float;
    words["true"] = new Word(init::TRUE, "true");
    words["false"] = new Word(init::FALSE, "false");
    words["return"] = new Word(init::RETURN, "return");
    words["println_int"] = Word::PRINT_INT;
}

void Lexer::readch() {
    if (position < len) {
        peek = input[position];
        position++;
    } else {
        peek = EOF;
    }
}

bool Lexer::readch(char c) {
    readch();
    if (peek != c) {
        return false;
    }
    peek = ' ';
    return true;
}

Token* Lexer::scan() {
    for (;; readch()) {
        if (peek == ' ' || peek == '\t' || peek == '\r') continue;
        else if (peek == '\n') {
            lines ++;
            continue;
        }
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
    } else if (peek == '~') {
        if (readch('=')) return new Word(init::NE, "~=");
        else return new Token('~');
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
    Token* Tok;
    if (peek == EOF) {
        Tok = new Token(init::END);
    } else Tok = new Token(peek);
    peek = ' ';
    return Tok;
}