#include <fxcg/keyboard.h>
#include <fxcg/display.h>
#include <fxcg/heap.h>
#include <fxcg/rtc.h>

#define MODULUS 0x100000000 // 2^32
#define MULTIPLIER 22695477
#define INCREMENT 1

unsigned int seed = RTC_GetTicks();

unsigned char rand_uint8()
{
    seed = (MULTIPLIER * seed + INCREMENT) % MODULUS;
    return (seed >> 16) & 0b11111111;
}

#define STATUS_AREA_HEIGHT 24
#define FKEY_AREA_HEIGHT 0

const color_t COLOR_BACKGROUND = COLOR_WHITE;
const color_t COLOR_NORMAL = COLOR_BLUE;
const color_t COLOR_SELECTED = COLOR_RED;

class ElementDrawer
{
    private:
        unsigned char offset, spacing;
        unsigned char width, height;
    public:
        void resize(unsigned char size)
        {
            width = LCD_WIDTH_PX * 9 / (10 * size);
            spacing = (LCD_WIDTH_PX - width * size) / (size - 1);
            offset = (LCD_WIDTH_PX - width * size - spacing * (size - 1)) / 2;
            height = (LCD_HEIGHT_PX - STATUS_AREA_HEIGHT - FKEY_AREA_HEIGHT) / size;
        }
        void drawElement(unsigned char pos, unsigned char value, color_t color)
        {
            color_t *VRAM = (color_t *)GetVRAMAddress();
            VRAM += (LCD_HEIGHT_PX - FKEY_AREA_HEIGHT - 1) * LCD_WIDTH_PX + offset + pos * spacing + (pos + 1) * width - 1;
            for (unsigned char i = value * height; i; i--)
            {
                for (unsigned char j = width; j; j--)
                {
                    *VRAM-- = color;
                }
                VRAM -= LCD_WIDTH_PX - width;
            }
        }
        void drawDifference(unsigned char pos, unsigned char smaller, unsigned char bigger, color_t color)
        {
            color_t *VRAM = (color_t *)GetVRAMAddress();
            VRAM += (LCD_HEIGHT_PX - FKEY_AREA_HEIGHT - smaller * height - 1) * LCD_WIDTH_PX + offset + pos * spacing + (pos + 1) * width - 1;
            for (unsigned char i = (bigger - smaller) * height; i; i--)
            {
                for (unsigned char j = width; j; j--)
                {
                    *VRAM-- = color;
                }
                VRAM -= LCD_WIDTH_PX - width;
            }
        }
};

class Sorter
{
    private:
        unsigned char *data = nullptr;
    public:
        unsigned char size;
        ElementDrawer drawer;
        void resize(unsigned char new_size)
        {
            size = new_size;
            drawer.resize(size);
            data = (unsigned char *)sys_realloc(data, sizeof(unsigned char) * size);
            for (unsigned char i = size; i; i--)
            {
                data[i-1] = i;
            }
        }
        void drawElements()
        {
            Bdisp_Fill_VRAM(COLOR_BACKGROUND, 1);
            for (int i = 0; i < size; i++)
            {
                drawer.drawElement(i, data[i], COLOR_NORMAL);
            }
        }
        void dispatch(unsigned char smaller, unsigned char bigger)
        {
            drawer.drawDifference(bigger, data[smaller], data[bigger], COLOR_BACKGROUND);
            drawer.drawElement(smaller, data[bigger], COLOR_SELECTED);
            drawer.drawElement(bigger, data[smaller], COLOR_SELECTED);
            Bdisp_PutDisp_DD();
            drawer.drawElement(smaller, data[bigger], COLOR_NORMAL);
            drawer.drawElement(bigger, data[smaller], COLOR_NORMAL);
        }
        void swap(unsigned char smaller, unsigned char bigger)
        {
            unsigned char temp = data[smaller];
            data[smaller] = data[bigger];
            data[bigger] = temp;
        }
        void heapify(unsigned char start, unsigned char length)
        {
            unsigned char max = start;
            unsigned short left = (start << 1) + 1;
            unsigned short right = (start << 1) + 2;

            if (left < length && data[left] > data[max])
                max = left;
            if (right < length && data[right] > data[max])
                max = right;
            if (max != start)
            {
                dispatch(start, max);
                swap(start, max);
                heapify(max, length);
            }
        }
        void sort()
        {
            for (char i = size / 2 - 1; i >= 0; i--)
            {
                heapify(i, size);
            }
            for (unsigned char i = size - 1; i > 0; i--)
            {
                dispatch(i, 0);
                swap(0, i);
                heapify(0, i);
            }
        }
        void shuffle()
        {
            unsigned char j;
            for (unsigned char i = size - 1; i; i--)
            {
                j = (unsigned char)rand_uint8() % (i + 1);
                swap(j, i);
            };
        }
};

#define SIZE_INIT 32
#define SIZE_MAX 128
#define SIZE_MIN 16

int main()
{
    Sorter sorter;
    sorter.resize(SIZE_INIT);
    sorter.drawElements();
    int key;
    while (true)
    {
        GetKey(&key);
        if (key == KEY_CTRL_EXE)
        {
            sorter.sort();
        }
        else if (key == KEY_CHAR_MINUS && sorter.size < SIZE_MAX)
        {
            sorter.resize(sorter.size << 1);
            goto shuffle;
        }
        else if (key == KEY_CHAR_PLUS && sorter.size > SIZE_MIN)
        {
            sorter.resize(sorter.size >> 1);
            goto shuffle;
        }
        else if (key == KEY_CHAR_LOG)
        {
            sorter.sort();
            PrintXY(1, 1, "  Informatik GK 11", TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
            PrintXY(1, 2, "  ****** **********", TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
            PrintXY(1, 3, "  Erstellt am", TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
            PrintXY(1, 4, "  23.04.21", TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
            PrintXY(1, 5, "  [EXIT]", TEXT_MODE_TRANSPARENT_BACKGROUND, TEXT_COLOR_BLACK);
            locate_OS(1, 6);
            Cursor_SetFlashOn(0);
            while (key != KEY_CTRL_EXIT)
            {
                GetKey(&key);
            }
            Cursor_SetFlashOff();
            goto shuffle;
        }
        else if (key == KEY_CTRL_AC)
        {
            shuffle:
            sorter.shuffle();
            sorter.drawElements();
        }
    }
    return 0;
}
