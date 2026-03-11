#pragma once



enum class FrontendErr {

    NONE = 0,
    INVALID_SYNTAX,

    UNKNOWN,

};



#define IF_FRONT_ERR_RET_                         \
    do {                                          \
        if (err.code != (int)FrontendErr::NONE)   \
            return (FrontendErr)err.code;         \
                                                  \
    } while(0)



#define SET_FRONTEND_ERR_DATA_(id, mes, ...)       \
    do                                             \
    {                                              \
        (err)->code = (int) id;                    \
        SetErrMes(err, mes, __VA_ARGS__);          \
        return (FrontendErr) (err)->code;          \
    } while(0)

#define EXIT_IF_FRONTEND_ERR_                      \
    do {                                           \
        if((err).code != (int)FrontendErr::NONE) { \
            printf("%s", (err).data);              \
            exit(0);                               \
        }                                          \
    } while(0);
    