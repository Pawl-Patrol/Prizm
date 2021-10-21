#include <fxcg/display.h>

void getRGB(color_t color, int *r, int *g, int *b);
color_t getColorAlpha(int r, int g, int b, float alpha);
void plot(int x, int y, color_t color);
void plotLineAA(int x0, int y0, int x1, int y1, color_t color);
void drawFKey(int pos, const bool* bitmap);