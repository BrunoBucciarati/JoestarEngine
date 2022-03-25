#pragma once
#include "stdio.h"
#include "windows.h"
#define LOG(fmt, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);\
    printf(fmt, ##__VA_ARGS__);
#define LOGERROR(fmt, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);\
    printf(fmt, ##__VA_ARGS__);
#define LOGWARN(fmt, ...) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);\
    printf(fmt, ##__VA_ARGS__);
