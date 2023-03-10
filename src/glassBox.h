#pragma once
#pragma comment(lib, "wtsapi32") 

extern char ok[];
extern char in[];
extern char er[];

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <WtsApi32.h>

#define KRST "\033[0m"
#define KGRN "\x1B[32m"
#define KRED "\x1B[31m"
#define KYEL "\x1B[33m"