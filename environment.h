#pragma once

#include <iostream>
#include <unordered_map>
#include "symbols.h"
#include "node.h"

class Env {
private:
    std::unordered_map<Token*, Id*> table;
public:
    Env* prev;
    Env(Env* n);
    void put(Token* w, Id* i);
    Id* get(Token* w);
};