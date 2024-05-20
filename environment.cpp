#include "environment.h"

Id* Env::get(Token* w) {
    for (Env* e = this; e != nullptr; e = e->prev) {
        Id* found = e->table[w];
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

void Env::put(Token* w, Id* i) {
    table[w] = i;
}

Env::Env(Env* n) {
    prev = n;
}