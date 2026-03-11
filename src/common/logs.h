#pragma once

#include <sys/stat.h>
#include <stdarg.h>



#ifndef MAX_STR_LEN_
#define MAX_STR_LEN_ 1024
#endif

#ifndef LOG_FILE_
#define LOG_FILE_ "-fdump"
#endif

#ifdef _WIN32
    #include <direct.h>
    #define _MKDIR(name) _mkdir(name)
    #define PATH_SEP '\\'
#else
    #define FILE_MODE_ 0755
    #define _MKDIR(name) mkdir(name, FILE_MODE_)
    #define PATH_SEP '/'
#endif



typedef int (*dump_f)(FILE* stream, const void* stk,
                      const char* func, const char* file, const int line);

const char* GetGlobalLogDir();

int LogDump(dump_f dump_func, const void* stk,
            const char* func, const char* file, const int line);

int PrintLog     (const char *format, ...);
int CloseLogFile ();
int OpenLogFile  (const int argc, const char* argv[]);



#ifndef START_LOG_DUMP_
#define START_LOG_DUMP_(file, line, func)                           \
    do {                                                            \
        PrintLog("=============LOG CALLED==============<br>\n"      \
                 "[RUNNING FUNC: %s FROM FILE \"%s\"(%d)]<br>\n",   \
                 func, file, line);                                 \
    }while(0)                  

#endif 

#ifndef END_LOG_DUMP_
#define END_LOG_DUMP_                     \
    do {                                  \
        PrintLog("[END TIME: %s]<br>\n"); \
    } while(0)

#endif