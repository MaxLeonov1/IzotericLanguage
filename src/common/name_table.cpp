#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "name_table.h"



NameTable_t* NametableCtor() {

    NameTable_t* nametable = (NameTable_t*)calloc(1, sizeof(NameTable_t));

    NametableFill(nametable);

    return nametable;

}



void NametableFill(NameTable_t* nametable) {

    assert(nametable);

    Name_t* list = (Name_t*) calloc(DEF_NAMES_AMT_, sizeof(Name_t));

    assert(list);

    nametable->buff     = list;
    nametable->size     = 0;
    nametable->capacity = DEF_NAMES_AMT_;

}



void NametableDtor(NameTable_t* nametable) {

    assert(nametable);
    assert(nametable->buff);

    for (size_t i = 0; i < nametable->size; i++) {

        if (nametable->buff[i].name != nullptr)
            free(nametable->buff[i].name);
    }

    free(nametable->buff);

    nametable->size     = 0;
    nametable->capacity = 0;

    free(nametable);
}



int InsertNameInTable(NameTable_t* nametable, const char* name, TokenType type ) {

    assert(nametable);
    assert(name);
    assert(type);

    for (size_t i = 0; i < nametable->size; i++ ) {

        if (!strncmp(name, nametable->buff[i].name, MAX_NAME_LEN_))
            return (int)i;

    }

    char* inserted_name = strdup(name);
    assert(inserted_name);

    nametable->buff[nametable->size].name = inserted_name;
    nametable->buff[nametable->size].type = type;

    return nametable->size++;

}



#define OPER_(code, name, ...) \
    InsertNameInTable(nametable, name, TokenType::TOKEN);

void FillNameTableWithKeyWords (NameTable_t* nametable) {

    assert(nametable);

    #include "oper_instr.h"
;  //WTF?
}

#undef OPER_



#define OPER_(code, name, ...)                     \
    if (!strncmp(keyword, name, MAX_NAME_LEN_)) {  \
        return Oper_t::code;                       \
    } else

Oper_t KeyWordToType (char* keyword) {

    #include "oper_instr.h"

    /*else*/ return Oper_t::UNK;

}

#undef OPER_



#define OPER_(code, ...)                        \
    if (!strncmp(word, #code, MAX_NAME_LEN_)) {  \
        return Oper_t::code;                    \
    }                                           \
    else


Oper_t GetOperator(const char* word) {

    if (!word) return Oper_t::UNK;

    #include "oper_instr.h"

    if (!strncmp(word, "FUNC_CALL", MAX_NAME_LEN_)) {
        return Oper_t::CALL_FUNC;
    }
    else if (!strncmp(word, "NEW_FUNC", MAX_NAME_LEN_)) {
        return Oper_t::NEW_FUNC;
    }
    else
        return Oper_t::UNK;

}

#undef OPER_



#define OPER_(code, ...)     \
    case Oper_t::code:       \
        fprintf(fp, #code);  \
        break; 

void PrintOper(FILE* fp, const Oper_t code) {

    switch(code) {

        #include "oper_instr.h"

        case Oper_t::NEW_FUNC:
            fprintf(fp, "NEW_FUNC");
            break;
        
        case Oper_t::CALL_FUNC:
            fprintf(fp, "CALL_FUNC");
            break;
    
        case Oper_t::PROGRAM_END:
            fprintf(fp, "PROGRAM_END");
            break;

        case Oper_t::UNK:
        default:
            fprintf(fp, "undef oper");
            break;

    }

}

#undef OPER_



#define OPER_(code, ...)  \
    case Oper_t::code:    \
        return #code; 
        
const char* OperToStr (const Oper_t code) {

    switch(code) {

        #include "oper_instr.h"

        case Oper_t::NEW_FUNC:
            return "NEW_FUNC";
        
        case Oper_t::CALL_FUNC:
            return "CALL_FUNC";
    
        case Oper_t::PROGRAM_END:
            return "PROGRAM_END";

        case Oper_t::UNK:
        default:
            return "UNK";

    }

}
        
#undef OPER_

void DumpNametable(FILE* fp, const NameTable_t* nametable) {

    assert(nametable);
    assert(nametable->buff);

    fprintf(fp, "NAMETABLE SIZE > %lu\n", nametable->size);

    for (size_t i = 0; i < nametable->size; i++) {

        if (nametable->buff[i].name != nullptr)
            fprintf(fp, "\"%s\"[%lu]\n", nametable->buff[i].name, i);
    }
}



void CopyNameTable (NameTable_t* source, NameTable_t* dest) {

    assert(source);
    assert(dest);

    for (size_t i = 0; i < dest->size; i++ ) {
        if (dest->buff[i].name != nullptr) {
            free(dest->buff[i].name);
        }
    }

    free(dest->buff);

    dest->size = source->size;
    dest->capacity = source->capacity;

    dest->buff = (Name_t*)calloc(dest->capacity, sizeof(Name_t));
    assert(dest->buff);

    for (size_t i = 0; i < source->size; i++ ) {
        dest->buff[i].type = source->buff[i].type;
        dest->buff[i].name = strdup(source->buff[i].name);
        if (!dest->buff[i].name) {

            for (size_t j = 0; j < i; j++) free(dest->buff[j].name);

            free(dest->buff);
            dest->buff = nullptr;
            dest->size = 0;
            dest->capacity = 0;
            return;
        }
    }
}



bool FindNameInTable(const NameTable_t* nametable, const char* name, bool* exists, bool* is_func) {

    assert(nametable);
    assert(name);
    assert(is_func);
    assert(exists);

    for (int i = 0; i < nametable->size; i++) {

        if (!strncmp(name, nametable->buff[i].name, MAX_NAME_LEN_)) {
            if (nametable->buff[i].type == TokenType::FUNC_NAME)
                *is_func = true;
            *exists = true;
            return true;
        }
    }

    return false;
}