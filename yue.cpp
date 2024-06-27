#include <iostream>
#include <assert.h>
#include "lexer.h"
#include "abstract_syntax_tree.h"
int main(int argc, char const *argv[]) {
    assert(argc > 1);
    std::fstream fs;
    fs.open(argv[1], std::ios::in);
    assert(fs.is_open());
    std::string code(std::istreambuf_iterator<char>(fs), {});
    Lexer lexer = Lexer(code);
    Parser parser = Parser(&lexer);
    parser.program();
    // while (true) {
    //     Token* token = lexer.scan();
    //     std::cout << token -> to_string() << std::endl;
    //     if (token -> type == init::END) {
    //         break;
    //     }
    // }
}