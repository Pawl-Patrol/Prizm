#include <fxcg/display.h>

void drawFKey(int id, int pos)
{
    void *bitmap;
    GetFKeyPtr(id, &bitmap);
    FKey_Display(pos, bitmap);
}

void fill(int x, int y, int w, int h, color_t color)
{
    color_t *VRAM = (color_t *)GetVRAMAddress();
    VRAM += y * LCD_WIDTH_PX + x;
    while (h--)
    {
        int w2 = w;
        while (w2--)
        {
            *VRAM++ = color;
        }
        VRAM += LCD_WIDTH_PX - w;
    }
}