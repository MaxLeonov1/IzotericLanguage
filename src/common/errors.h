#pragma once

#define MAX_ERR_LEN_ 100



struct Err_t {

    int         code;
    const char* data;

};



enum class ErrCode {

    NONE = 0,
    OPEN_FILE,
    READ_FILE,
    MEM_ALLOC,

    UNKNOWN,

};



int SetErrMes (Err_t* err, const char* format, ...);