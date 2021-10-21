// Include file for textinput.c
// Created by Christopher Mitchell / Kerm Martian
// http://www.cemetech.net
#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/rtc.h>
#include <string.h>

#define INPUT_MODE_TEXT 0
#define INPUT_MODE_FLOAT 1
#define INPUT_MODE_INT 2
#define INPUT_MODE_POSINT 3

#define CURSOR_FLASH_RATE 32
#define KEY_PRGM_DEL 44

int getTextLine(char* buf, int maxstrlen, int x, int y, int maxdisplen, unsigned short inmode);
void DrawCursor(int x, int y, int shiftmode, int alphamode, int cursorstate, char curchar);