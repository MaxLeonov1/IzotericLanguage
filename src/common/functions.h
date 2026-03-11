#pragma once



unsigned int djb2hash         (const char* str);
int          skip_space       (char** ptr);
long long    FileByteCount    (const char* filename);
char*        my_strdup        (const char* str);
int          double_cmp       (double double_1, double double_2);
void         reset_input_buff ();