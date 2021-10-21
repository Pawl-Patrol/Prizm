#include "draw.hpp"
#include "math.hpp"

unsigned char FKEY_WIDTH = LCD_WIDTH_PX / 6;
unsigned char FKEY_HEIGHT = LCD_HEIGHT_PX / 9;


void getRGB(color_t color, int *r, int *g, int *b)
{
    *r = color >> 11;
    *g = (color & 0x07E0) >> 5;
    *b = color & 0x001F;
}


color_t getColorAlpha(int r, int g, int b, float alpha)
{
    r = (1.0f - alpha) * r + alpha * 31;
    g = (1.0f - alpha) * g + alpha * 63;
    b = (1.0f - alpha) * b + alpha * 31;
    return r << 11 | g << 5 | b;
}

void plot(int x, int y, color_t color)
{
    color_t *VRAM = (color_t *)GetVRAMAddress();
    VRAM[y * LCD_WIDTH_PX + x] = color;
}

void plotLineAA(int x0, int y0, int x1, int y1, color_t color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int x2, e2, err = dx - dy;
    int ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);
    int r, g, b;
    getRGB(color, &r, &g, &b);
    while (true)
    {
        plot(x0, y0, getColorAlpha(r, g, b, (float)abs(err - dx + dy) / ed));
        e2 = err;
        x2 = x0;
        if (2 * e2 >= -dx)
        {
            if (x0 == x1)
                break;
            if (e2 + dy < ed)
                plot(x0, y0 + sy, getColorAlpha(r, g, b, (float)(e2 + dy) / ed));
            err -= dy;
            x0 += sx;
        }
        if (2 * e2 <= dy)
        {
            if (y0 == y1)
                break;
            if (dx - e2 < ed)
                plot(x2 + sx, y0, getColorAlpha(r, g, b, (float)(dx - e2) / ed));
            err += dx;
            y0 += sy;
        }
    }
}

void drawFKey(int pos, const bool* bitmap)
{
    color_t *VRAM = (color_t *)GetVRAMAddress();
    int shift = LCD_WIDTH_PX - FKEY_WIDTH;
    VRAM += LCD_WIDTH_PX * (LCD_HEIGHT_PX-FKEY_HEIGHT) + pos * FKEY_WIDTH;
    for (int y = 0; y < FKEY_HEIGHT; y++)
    {
        for (int x = 0; x < FKEY_WIDTH; x++)
        {
            *(VRAM++) = *(bitmap++) ? COLOR_BLACK : COLOR_WHITE;
        }
        VRAM += shift;
    }
}