#include "converters.hpp"

#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/heap.h>

int getChoice(const char* title, const  char** choices, int length);
bool getValue(float *value, int x, int y, const char *msg, int maxlen, int *precision);