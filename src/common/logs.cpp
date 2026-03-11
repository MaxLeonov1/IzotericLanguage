#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "logs.h"
#include "colors.h"

static FILE*       LOG_STREAM_ = nullptr;
static const char* EXTENSION   = ".log.html";

/*=====================================================================================*/

static int IsLogFlag (const int argc, const char* argv[], const char* flag);

/*=====================================================================================*/

const char* GetGlobalLogDir() {
    static char log_dir[MAX_STR_LEN_] = "";
    
    if (log_dir[0] == '\0') {
        char proj_path[MAX_STR_LEN_] = "";
        time_t log_time_sec = time(NULL);
        struct tm* log_time = localtime(&log_time_sec);
        
        getcwd(proj_path, sizeof(proj_path));
        sprintf(proj_path, "%s%clogs", proj_path, PATH_SEP);
        _MKDIR(proj_path);

        sprintf(log_dir,
                "%s%clog_%d.%d.%d_%d-%d-%d",
                proj_path,
                PATH_SEP,
                log_time->tm_mday,
                log_time->tm_mon,
                1900 + log_time->tm_year,
                log_time->tm_hour,
                log_time->tm_min,
                log_time->tm_sec);
        _MKDIR(log_dir);
    }
    
    return log_dir;
}

/*=====================================================================================*/

int OpenLogFile (const int argc, const char* argv[]) {

    assert(argv);

    bool is_log = IsLogFlag(argc, argv, LOG_FILE_);
    
    if (!is_log)
        return 0;

    const char* FILE_NAME_ = argv[0];

    char file_name[MAX_STR_LEN_] = "";
    snprintf(file_name, sizeof(file_name), "%s%s", FILE_NAME_, EXTENSION);
    //printf("%s\n", file_name);

    LOG_STREAM_ = fopen(file_name, "w");

    if (LOG_STREAM_ == nullptr)
        LOG_STREAM_ =  stderr;

    time_t now = 0;
    time(&now);

    fprintf(LOG_STREAM_, 
            "<br>\n=========================== PROGRAM START ===========================<br>\n"
            "RUNNED AT %s<br>\n", ctime(&now));
    fflush(LOG_STREAM_);

    return 0;

}

/*=====================================================================================*/

int CloseLogFile() {
 
    fprintf(LOG_STREAM_,
            "============================ PROGRAM END ============================<br>\n");
    fflush(LOG_STREAM_);
    fclose(LOG_STREAM_);

    return 0;

}

/*=====================================================================================*/

static int IsLogFlag (const int argc, const char* argv[], const char* flag) {

    assert(argv);
    assert(flag);

    if (argc > 1) {

        for (size_t i = 1; i < argc; i++) {

            if (!strncmp(flag, argv[i], MAX_STR_LEN_))
                return true;
        }
    }

    return false;

}

/*=====================================================================================*/

int LogDump(dump_f dump_func, const void* stk,
            const char* func, const char* file, const int line) {

    if (!LOG_STREAM_)
        return 0;

    assert(dump_func);
    assert(stk);
    int result = dump_func(LOG_STREAM_, stk, func, file, line);
    
    return result;
}



int PrintLog (const char *format, ...) {

    if (LOG_STREAM_ == nullptr)
        return 0;

    va_list arg;
    int done = 0;

    va_start (arg, format);
    done = vfprintf(LOG_STREAM_, format, arg);
    fflush(LOG_STREAM_);
    va_end (arg);

    return done;
}