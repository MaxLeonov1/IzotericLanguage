#pragma once

#include "../tree/tree.h"
#include "../common/read_file.h"
#include "frontend.h"
#include "../stack/stack.h"



#define DEF_TOKENS_NUM_ 500



typedef struct TokenArr {

    Node** nodes;
    size_t size;
    size_t ptr;

    NameTable_t* nametable;
    Stack_t      nametable_stk;

};



void DumpTokensStorage(FILE* fp, TokenArr* tokens);
void DumpToken(FILE* fp, Node* token, NameTable_t* nametable);

void TokenArrCtor(TokenArr* tokens);
void TokenArrDtor(TokenArr* tokens);

FrontendErr LexAnalysis (TextStorage* text, TokenArr* tokens, Err_t* err);