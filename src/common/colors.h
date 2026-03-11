#pragma once


#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define STD     "\033[0m"

#define PrintBlackText(fp, string, ...)   fprintf(fp, BLACK   string STD, __VA_ARGS__)
#define PrintRedText(fp, string, ...)     fprintf(fp, RED     string STD, __VA_ARGS__)
#define PrintGreenText(fp, string, ...)   fprintf(fp, GREEN   string STD, __VA_ARGS__)
#define PrintYellowText(fp, string, ...)  fprintf(fp, YELLOW  string STD, __VA_ARGS__)
#define PrintBlueText(fp, string, ...)    fprintf(fp, BLUE    string STD, __VA_ARGS__)
#define PrintMagentaText(fp, string, ...) fprintf(fp, MAGENTA string STD, __VA_ARGS__)
#define PrintCyanText(fp, string, ...)    fprintf(fp, CYAN    string STD, __VA_ARGS__)
#define PrintWhiteText(fp, string, ...)   fprintf(fp, WHITE   string STD, __VA_ARGS__)