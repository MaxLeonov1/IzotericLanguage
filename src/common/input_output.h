#pragma once

#include <stdio.h>

#include "errors.h"
#include "colors.h"



FILE*       OpenFile(const char* file_name, const char* mode, Err_t* error);
const char* GetFileName(int argc, char const *argv[], const int id, const char* mode);