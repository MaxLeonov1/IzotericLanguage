#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "read_file.h"
#include "functions.h"
#include "colors.h"



static void AllocBuffer (FILE* fp, size_t byte_num, char** buff, Err_t* err);
static void SetParams   (TextStorage* text, size_t byte_num, char* buffer);


static void AllocBuffer (FILE* fp, size_t byte_num, char** buff, Err_t* err) {

    assert(fp);
    assert(err);

    *buff = (char* ) calloc(byte_num + 1, sizeof(char));

    if (!*buff)
        err->code = (int)ErrCode::MEM_ALLOC;

    size_t symbols_read = fread(*buff, sizeof(char), byte_num, fp);

    if (symbols_read != byte_num)
        err->code = (int)ErrCode::READ_FILE;

}



static void SetParams (TextStorage* text, size_t byte_num, char* buffer) {

    assert(text);

    text->buffer = buffer;
    text->cur_line = 1;
    text->ptr = 0;
    text->text_len = byte_num;
}



int TextStorageCtor (TextStorage* text, Err_t* err, const char* filename) {

    assert(text);
    assert(err);

    FILE* fp = fopen(filename, "rb");

    if (!fp) {

        err->data = filename;
        err->code = (int)ErrCode::OPEN_FILE;
        return err->code;
    }

    size_t byte_num = (size_t)FileByteCount(filename);
    char* buffer = nullptr;

    AllocBuffer(fp, byte_num, &buffer, err);

    //PrintRedText(stderr, "buffer:%p\n", buffer);

    if (err->code != (int)ErrCode::NONE)
        return err->code;

    SetParams(text, byte_num, buffer);

    //printf("%p\n", text->buffer);
    fclose(fp);
    return (int)err->code;

}



void TextStorageDtor (TextStorage* text) {

    assert(text);

    //printf("%p\n", text->buffer);
    free(text->buffer);
}



int Buffgetc (TextStorage* text) {

    assert(text);

    int ch = text->buffer[text->ptr];
    text->ptr++;

    if (ch == '\n')
        text->cur_line++;

    return ch;

}



int BufScanfWord(TextStorage* text, char* dest) {

    assert(dest);

    int symb_amt = 0;
    int done = sscanf(text->buffer + text->ptr, "%s%n", dest, &symb_amt);
    text->ptr += symb_amt;

    return done;
}



bool SkipSpaces(TextStorage* text) {

    int ch = 0;
    bool skipped = false;

    ch = Buffgetc(text);

    while (isspace(ch)) {

        ch = Buffgetc(text);
        skipped = true;
    }

    text->ptr--;

    return skipped;
}