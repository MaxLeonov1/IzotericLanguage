#pragma once

#include <stdlib.h>

#include "errors.h"



#define MAX_STR_LEN_ 100



struct TextStorage {

    char*  buffer;
    size_t ptr;
    size_t text_len;
    size_t cur_line;

};



int  Buffgetc        (TextStorage* text);
int  TextStorageCtor (TextStorage* text, Err_t* err, const char* filename);
void TextStorageDtor (TextStorage* text);
int  BufScanfWord    (TextStorage* text, char* dest);
bool SkipSpaces      (TextStorage* text);