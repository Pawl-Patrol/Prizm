#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>
#include <fxcg/heap.h>

#define STATUS_AREA_HEIGHT 25
#define FRAME_AREA_HEIGHT 4
#define MINI_FONT_HEIGHT 18
#define SHUFFLE_ITERATIONS_MIN 15
#define SHUFFLE_ITERATIONS_MAX 35
#define CELL_AMOUNT_MAX 9
#define CELL_AMOUNT_MIN 3
#define SHUFFLE_DELAY 100

const char* keybindings[7] = {"[F1] Auswahl", "[AC] Zufall", "[DEL] Bereinigen", "[OPTN] Farbmodus", "[+] Hereinzoomen", "[-] Herauszoomen", "[MENU] Verlassen"};

// COLORS
const color_t COLOR_BACKGROUND = COLOR_WHITE;
const color_t COLOR_GRID = COLOR_GRAY;
const color_t COLOR_CELL_ON = COLOR_DARKGOLDENROD;
const color_t COLOR_CELL_OFF = COLOR_DARKGRAY;
const color_t COLOR_SELECTOR = COLOR_CORNFLOWERBLUE;
const color_t COLOR_FONT_BG = COLOR_DARKGRAY;
bool color_mode = true;

// SIZES AND OFFSETS
unsigned char height = LCD_HEIGHT_PX - STATUS_AREA_HEIGHT;
unsigned char border;
unsigned char cellXY;
unsigned char sizeXY;

// GAME
bool mapping[CELL_AMOUNT_MAX][CELL_AMOUNT_MAX];
unsigned char cells;
unsigned char selectorX;
unsigned char selectorY;

// CONTROLS
int controls[4] = {KEY_CTRL_UP, KEY_CTRL_DOWN, KEY_CTRL_RIGHT, KEY_CTRL_LEFT};
const unsigned short* keyboard = (unsigned short*)0xA44B0000;

// KEYBOARD HISTORY FOR RANDOM NUMBERS
unsigned int randHistory[SHUFFLE_ITERATIONS_MAX];
unsigned char randIndex = 0;

void appendToHistory()
{
    randHistory[randIndex] = RTC_GetTicks();
    randIndex = (randIndex + 1) % SHUFFLE_ITERATIONS_MAX;
}

bool keydown(int keycode) {
    int row, col, word, bit; 
    row = keycode % 10; 
    col = keycode / 10 - 1; 
    word = row >> 1; 
    bit = col + 8 * (row & 1); 
    return 0 != (keyboard[word] & 1 << bit); 
}

void plotRect(unsigned x, unsigned y, unsigned w, unsigned h, color_t color)
{
    color_t *VRAM = (color_t *)GetVRAMAddress();
    VRAM += (y * 384) + x;
    while (h--)
    {
        unsigned w2 = w;
        while (w2--)
            *VRAM++ = color;
        VRAM += 384 - w;
    }
}

void plotCell(unsigned char x, unsigned char y)
{
    plotRect(border * (x + 1) + cellXY * x, STATUS_AREA_HEIGHT + border * (y + 1) + cellXY * y, cellXY, cellXY, mapping[x][y] ? COLOR_CELL_ON : COLOR_CELL_OFF);
}

void plotSelector(color_t color)
{
    plotRect(selectorX * (border + cellXY), STATUS_AREA_HEIGHT + selectorY * (border + cellXY), cellXY + border * 2, cellXY + border * 2, color);
    plotCell(selectorX, selectorY);
}

void toggleCell(unsigned char x, unsigned char y)
{
    mapping[x][y] = !mapping[x][y];
    plotCell(x, y);
}

void toggleNeighbors(unsigned char x, unsigned char y)
{
    if (x > 0)
    {
        toggleCell(x - 1, y);
    }
    if (x < cells - 1)
    {
        toggleCell(x + 1, y);
    }
    if (y > 0)
    {
        toggleCell(x, y - 1);
    }
    if (y < cells - 1)
    {
        toggleCell(x, y + 1);
    }
}

int pseudoRand(int min, int max)
{
    unsigned int now = RTC_GetTicks();
    unsigned char key = now % SHUFFLE_ITERATIONS_MAX;
    unsigned int seed = randHistory[key];
    return min + (now / 3 + seed * seed) % (max - min);
}

void shuffle()
{
    int time = RTC_GetTicks();
    int iterations = pseudoRand(SHUFFLE_ITERATIONS_MIN, SHUFFLE_ITERATIONS_MAX);
    for (int i = 0; i < iterations; i++)
    {
        plotSelector(COLOR_GRID);
        selectorX = pseudoRand(0, cells);
        time = RTC_GetTicks();
        while (!RTC_Elapsed_ms(time, SHUFFLE_DELAY + (selectorX - cells / 2) * 7))
        {
            if (keydown(KEY_PRGM_MENU) || keydown(KEY_PRGM_EXIT))
            {
                return;
            }
        }
        selectorY = pseudoRand(0, cells);
        toggleCell(selectorX, selectorY);
        toggleNeighbors(selectorX, selectorY);
        plotSelector(COLOR_SELECTOR);
        Bdisp_PutDisp_DD();
    }
}

void clear(bool state)
{
    for (int x = 0; x < cells; x++)
    {
        for (int y = 0; y < cells; y++)
        {
            mapping[x][y] = state;
            plotCell(x, y);
        }
    }
}

void init(int size)
{
    plotRect(0, STATUS_AREA_HEIGHT, sizeXY, sizeXY, COLOR_BACKGROUND);
    cells = size;
    border = height / cells / 10;
    cellXY = (height - border * (cells + 1)) / cells;
    sizeXY = cellXY * cells + border * (cells + 1);
    selectorX = cells / 2;
    selectorY = selectorX;
}

void draw()
{
    plotRect(0, STATUS_AREA_HEIGHT, sizeXY, sizeXY, COLOR_GRID);
    plotSelector(COLOR_SELECTOR);
}

int main(void)
{
    DefineStatusAreaFlags(3, SAF_BATTERY | SAF_ALPHA_SHIFT | SAF_TEXT, 0, 0);
    DefineStatusMessage("Der Feind sieht Dein Licht!", 0, TEXT_COLOR_BLACK, 0);
    Bdisp_EnableColor(color_mode);
    init(5);
    int x, y = (LCD_HEIGHT_PX - STATUS_AREA_HEIGHT - 7 * MINI_FONT_HEIGHT) / 2;
    for (int i = 0; i < sizeof(keybindings) / sizeof(char*); i++)
    {
        x = sizeXY + FRAME_AREA_HEIGHT + 5;
        PrintMini(&x, &y, keybindings[i], 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
        y += MINI_FONT_HEIGHT;
    }
    draw();
    clear(true);
    int key;
    while (true)
    {
        GetKey(&key);
        if (key == controls[0] || key == controls[1] || key == controls[2] || key == controls[3])
        {
            plotSelector(COLOR_GRID);
            if (key == controls[0])
            {
                selectorY = (selectorY - 1 + cells) % cells;
            }
            else if (key == controls[1])
            {
                selectorY = (selectorY + 1) % cells;
            }
            else if (key == controls[2])
            {
                selectorX = (selectorX + 1) % cells;
            }
            else if (key == controls[3])
            {
                selectorX = (selectorX - 1 + cells) % cells;
            }
            plotSelector(COLOR_SELECTOR);
        }
        else if (key == KEY_CTRL_F1)
        {
            toggleCell(selectorX, selectorY);
            toggleNeighbors(selectorX, selectorY);
        }
        else if (key == KEY_CTRL_AC)
        {
            shuffle();
        }
        else if (key == KEY_CTRL_DEL)
        {
            clear(false);
        }
        else if (key == KEY_CTRL_OPTN)
        {
            color_mode = !color_mode;
            Bdisp_EnableColor(color_mode);
        }
        else if (key == KEY_CHAR_PLUS && cells > 3)
        {
            init(cells-1);
            draw();
            clear(false);
        }
        else if (key == KEY_CHAR_MINUS && cells < CELL_AMOUNT_MAX)
        {
            init(cells+1);
            draw();
            clear(false);
        }
        appendToHistory();
    }
    return 0;
}
