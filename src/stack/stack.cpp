#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"



static const Stk_elem_t POISON_NUM = NULL;

/*-----------------------------------------------------------------------------------------------*/

StkErr StackCtor ( Stack_t* stack, size_t capacity ) {

    assert (stack);

    stack->capacity = capacity;
    stack->size = 0;
    stack->data = (Stk_elem_t*) calloc ( capacity, sizeof(Stk_elem_t) );

    if (!stack->data) return StkErr::STACK_ALLOCATION_ERROR;

    for (size_t ind = 0; ind < capacity; ind++) {
        stack->data[ind] = POISON_NUM;
    }
    
    return StkErr::HANDLED_SUCCSESFULY;
}

/*-----------------------------------------------------------------------------------------------*/

StkErr StackPush ( Stack_t* stack, Stk_elem_t value ) {

    assert (stack);

    if (stack->size >= stack->capacity) StackAllocation ( stack );

    stack->data[stack->size++] = value;

    return StkErr::HANDLED_SUCCSESFULY;
}

/*-----------------------------------------------------------------------------------------------*/

StkErr StackPop ( Stack_t* stack, Stk_elem_t* value ) {

    assert (stack);

    if(value)
        *(value) = stack->data[--(stack->size)];
    else
        stack->size--;

    NametableDtor((stack->data)[(stack->size)]);
    (stack->data)[(stack->size)] = POISON_NUM;

    return StkErr::HANDLED_SUCCSESFULY;

}

/*-----------------------------------------------------------------------------------------------*/

StkErr StackDtor ( Stack_t* stack ) {

    assert(stack);

    stack->capacity = 0;
    stack->size = 0;
    
    if (stack->data);
        free(stack->data);
    stack->data = nullptr;

    return StkErr::HANDLED_SUCCSESFULY;

}

/*-----------------------------------------------------------------------------------------------*/

StkErr StackAllocation ( Stack_t* stack ) {

    stack->data = (Stk_elem_t*) realloc ( stack->data, ( stack->capacity*2 ) * sizeof(Stk_elem_t) );

    for ( size_t ind = stack->size; ind < stack->capacity*2; ind++ ) stack->data[ind] = POISON_NUM;

    stack->capacity = stack->capacity*2;

}

/*-----------------------------------------------------------------------------------------------*/