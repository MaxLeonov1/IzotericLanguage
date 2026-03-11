#pragma once

#include "../tree/tree.h"
#include "../common/errors.h"



static const char* DEF_ASM_FILE = "src/assets/prog.txt";



enum class BackendErr {

    NONE = 0,
    INVALID_TREE,

    UNKNOWN,

};



#define IF_BACK_ERR_RET_                         \
    do {                                           \
        if (err.code != (int)BackendErr::NONE)   \
            return (BackendErr)err.code;         \
                                                   \
    } while(0)

#define SET_BACK_ERR_DATA_(id, mes, ...)         \
    do                                             \
    {                                              \
        (err)->code = (int) id;                    \
        SetErrMes(err, mes, __VA_ARGS__);          \
        return (BackendErr) (err)->code;         \
    } while(0)

#define EXIT_IF_BACK_ERR_                         \
    do {                                            \
        if((err).code != (int)BackendErr::NONE) { \
            printf("%s", (err).data);               \
            exit(0);                                \
        }                                           \
    } while(0);

void TreeToAsm (Tree_t* tree, Err_t* err, FILE* dest);