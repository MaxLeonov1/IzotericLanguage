#include <stdio.h>
#include <assert.h>

#include "input_output.h"



FILE* OpenFile(const char* file_name, const char* mode, Err_t* error) {

    assert(file_name);
    assert(error);
    assert(mode);

    FILE* fp = fopen(file_name, mode);
    if (!fp){
        error->code = (int) ErrCode::OPEN_FILE;
        error->data = file_name;
    }

    return fp;
}



const char* GetFileName (int argc, char const *argv[], const int id, const char* mode) {

    assert(argv);

    const char* filename = nullptr;

    if (argc>id)
        filename = argv[id];

    else {
        PrintBlueText(stdout, "ENTER %s FILE NAME: \n", mode);
        scanf("%s", filename);
    }

    if(filename)
        PrintGreenText(stdout, "%s FILE IS: %s\n", mode, filename);

    return filename;

}