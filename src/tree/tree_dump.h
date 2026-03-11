#pragma once

#include "tree.h"
#include "../common/errors.h"



int  TreeDump        (FILE* stream, const void* stk, 
                      const char* func, const char* file, const int line);
void PrintPrefixTree (FILE* fp, const Tree_t* tree);



#ifndef TREE_DUMP_
#define TREE_DUMP_(tree)                                              \
    do {                                                              \
        LogDump(TreeDump, (tree), __FUNCTION__, __FILE__, __LINE__);  \
    } while(0)                                                        \

#endif