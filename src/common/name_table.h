#pragma once

#include <stdlib.h>



#define OPER_(code, ...) code,

typedef enum{

    #include "../common/oper_instr.h"

    NEW_FUNC,
    CALL_FUNC,
    PROGRAM_END,
    UNK,

} Oper_t;

#undef OPER_

enum class TokenType {

    NUM,
    NAME,
    TOKEN,

    FUNC_NAME,
    VAR_NAME

};

struct Name_t {

    char* name;
    TokenType type;

};

struct NameTable_t {

    Name_t* buff;

    size_t size;
    size_t capacity;

};

#define MAX_NAME_LEN_ 64
#define DEF_NAMES_AMT_ 64



NameTable_t* NametableCtor();
void         NametableFill(NameTable_t* nametable);
void         NametableDtor(NameTable_t* nametable);
int          InsertNameInTable(NameTable_t* nametable, const char* name, TokenType type = TokenType::NAME );
Oper_t       KeyWordToType (char* keyword);
void         DumpNametable(FILE* fp, const NameTable_t* nametable);
void         PrintOper(FILE* fp, const Oper_t code);
void         FillNameTableWithKeyWords (NameTable_t* nametable);
const char*  OperToStr(const Oper_t code);
void         CopyNameTable (NameTable_t* source, NameTable_t* dest);
bool         FindNameInTable(const NameTable_t* nametable,
                             const char* name, bool* exists, bool* is_func);
Oper_t       GetOperator(const char* word);