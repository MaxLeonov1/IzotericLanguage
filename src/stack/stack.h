#pragma once

#include <stdlib.h>

#include "../common/name_table.h"



typedef NameTable_t* Stk_elem_t;

typedef struct {

    Stk_elem_t* data;
    size_t      size;
    size_t      capacity;

} Stack_t;

typedef enum {

    HANDLED_SUCCSESFULY = 0,  
    ZERO_CAPACITY_ERROR = 1,
    SIZE_OVERFLOW_ERROR = 2,
    STACK_ALLOCATION_ERROR = 3,
    ZERO_SIZE_ERROR = 4,
    INCORRECT_CONTROL_SUM_ERROR = 5,

} StkErr;



StkErr StackPush       (Stack_t* stack, Stk_elem_t value );
StkErr StackCtor       (Stack_t* stack, size_t capacity );
StkErr StackPop        (Stack_t* stack, Stk_elem_t* value = nullptr );
StkErr StackDtor       (Stack_t* stack );
StkErr StackAllocation (Stack_t* stack );
