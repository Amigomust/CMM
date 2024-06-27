# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# 定义编译选项
CXXFLAGS = -std=c++17 -Wall -DLOCAL
# 存储$(wildcard obj/*.o)的变量

all: lexer node parser cmm  

#=============================================================================
# Special targets provided by cmake.

lexer: lexer.h lexer.cpp symbols.h symbols.cpp
	$(CXX) $(CXXFLAGS) -c lexer.cpp symbols.cpp

node: node.h node.cpp environment.h environment.cpp
	$(CXX) $(CXXFLAGS) -c node.cpp environment.cpp

parser: abstract_syntax_tree.h abstract_syntax_tree.cpp
	$(CXX) $(CXXFLAGS) -c abstract_syntax_tree.cpp

cmm: abstract_syntax_tree.o yue.cpp node.o lexer.o symbols.o environment.o
	$(CXX) $(CXXFLAGS) -o csubsub symbols.o lexer.o node.o environment.o abstract_syntax_tree.o yue.cpp   