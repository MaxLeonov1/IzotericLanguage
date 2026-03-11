#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "functions.h"

/*=====================================================================================*/

unsigned int djb2hash ( const char* str ) {

    u_int hash = 5381;
    u_char c = 0;
    
    while ((c = (u_char)*str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/*=====================================================================================*/

int skip_space ( char** ptr ) {

    int sp_cnt = 0;

    while ( **ptr == ' ' ) {
        sp_cnt++;
        (*ptr)++;
    }
    return sp_cnt;

}

/*=====================================================================================*/

long long FileByteCount ( const char* filename ) {

    struct stat file_info = {};

    stat ( filename, &file_info );

    return file_info.st_size;

}

/*=====================================================================================*/

char* my_strdup(const char* str) {
    if (str == NULL) return NULL;
    
    size_t len = strlen(str) + 1;
    char* copy = (char*)calloc(len, sizeof(copy[0]));
    
    if (copy != NULL) {
        memcpy(copy, str, len);
    }
    
    return copy;
}

/*=====================================================================================*/

int double_cmp (double double_1, double double_2) {

    const double error = 1e-6;

    return (fabs (double_1 - double_2) < error) ? 1 : 0;
}

/*=====================================================================================*/

void reset_input_buff() {
    
    while (getchar() != '\n');

} 