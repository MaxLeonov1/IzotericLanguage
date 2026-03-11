#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "errors.h"



int SetErrMes (Err_t* err, const char* format, ...) {

    assert(err);
    
    char* data = (char*)calloc(MAX_ERR_LEN_, sizeof(char));
    assert(data);

    va_list arg;
    int printed = 0;

    va_start (arg, format);
    printed = vsnprintf(data, MAX_ERR_LEN_, format, arg);
    va_end (arg);

    err->data = data;

    return printed;
}