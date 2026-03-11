#pragma once

#include "../tree/tree.h"



enum class MiddleendErr {

    NONE = 0,
    INVALID_TREE,

    UNKNOWN,

};



#define IF_MIDDLE_ERR_RET_                         \
    do {                                           \
        if (err.code != (int)MiddleendErr::NONE)   \
            return (MiddleendErr)err.code;         \
                                                   \
    } while(0)

#define SET_MIDDLE_ERR_DATA_(id, mes, ...)         \
    do                                             \
    {                                              \
        (err)->code = (int) id;                    \
        SetErrMes(err, mes, __VA_ARGS__);          \
        return (MiddleendErr) (err)->code;         \
    } while(0)

#define EXIT_IF_MIDDLE_ERR_                         \
    do {                                            \
        if((err).code != (int)MiddleendErr::NONE) { \
            printf("%s", (err).data);               \
            exit(0);                                \
        }                                           \
    } while(0);


MiddleendErr SimplTree(Tree_t* tree);