#include <fxcg/keyboard.h>

#include "sudoku_gen.hpp"
#include "draw.hpp"
#include "random.hpp"

#define SIZE 24
#define OFFSET (LCD_WIDTH_PX - SIZE * 9) / 2
#define FONT_WIDTH 18
#define FONT_HEIGHT 24

#define COLOR(r, g, b) ((r * 31 / 255) << 11 | (g * 63 / 255) << 5 | (b * 31 / 255))
#define COLOR_INDEX COLOR(167, 215, 255)
#define COLOR_ERROR COLOR(247, 207, 214)
#define COLOR_SIMILAR COLOR(219, 236, 247)
#define COLOR_HIGHLIGHT COLOR(226, 235, 243)
#define COLOR_FONT_BOARD COLOR(53, 73, 98)
#define COLOR_FONT_PLAYER COLOR(53, 142, 229)
#define COLOR_FONT_ERROR COLOR(229, 92, 108)
#define COLOR_BORDER_DARK COLOR(59, 78, 103)
#define COLOR_BORDER_LIGHT COLOR(197, 205, 218)

#define TIMEOUT 1800

SudokuGen lasVegas(SudokuGen &sudoku)
{
    while (true)
    {
        HourGlass();
        memset(sudoku.board, EMPTY, sizeof(sudoku.board));
        sudoku.index = 0;
        sudoku.setAtIndex(RandInt(0, 10));
        char digits[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        for (int i = 0; i < 15; i++)
        {
            do
            {
                sudoku.index = RandInt(0, 81);
            } while (sudoku.getAtIndex() != EMPTY);
            shuffle(digits, 9);
            for (int j = 0; j < 9; j++)
            {
                if (sudoku.isPossible(sudoku.index % 9, sudoku.index / 9, digits[j]))
                {
                    sudoku.setAtIndex(digits[j]);
                    break;
                }
            }
            if (sudoku.getAtIndex() == EMPTY)
            {
                i--;
            }
        }
        sudoku.resetIndex();
        if (sudoku.solve(RTC_GetTicks() ,TIMEOUT))
        {
            return sudoku;
        }
    }
}

void dig(SudokuGen &sudoku)
{
    char positions[81];
    for (int i = 0; i < 81; i++)
    {
        positions[i] = i;
    }
    shuffle(positions, 81);
    for (int i = 0; i < 81; i++)
    {
        ProgressBar(20 + i, 100);
        sudoku.index = positions[i];
        char temp = sudoku.getAtIndex();
        sudoku.setAtIndex(EMPTY);
        int counter = 0;
        sudoku.countSolutions(counter, 2);
        if (counter > 1)
        {
            sudoku.setAtIndex(temp);
            sudoku.toggleIndex(true);
        }
    }
}

void draw(Sudoku sudoku, int x, int y, bool style)
{
    Bdisp_Fill_VRAM(COLOR_WHITE, 3);
    if (style)
    {
        fill(OFFSET, y * SIZE, SIZE * 9, SIZE, COLOR_HIGHLIGHT);
        fill(OFFSET + x * SIZE, 0, SIZE, SIZE * 9, COLOR_HIGHLIGHT);
        fill(OFFSET + x / 3 * 3 * SIZE, y / 3 * 3 * SIZE, SIZE * 3, SIZE * 3, COLOR_HIGHLIGHT);
    }
    fill(OFFSET + x * SIZE, y * SIZE, SIZE, SIZE, style ? COLOR_INDEX : COLOR_BLACK);
    char value = sudoku.board[x][y].value;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            Field field = sudoku.board[i][j];
            if (field.value)
            {
                bool valid = sudoku.isPossible(i, j, field.value);
                bool idx = i == x && j == y;
                if (style && !idx && (!valid || field.value == value))
                {
                    fill(OFFSET + i * SIZE, j * SIZE, SIZE, SIZE, valid ? COLOR_SIMILAR : COLOR_ERROR);
                }
                char buffer[2] = {'0' + field.value, '\0'};
                PrintCXY(OFFSET + i * SIZE + (SIZE - FONT_WIDTH) / 2, j * SIZE + (j ? -22 : 2), buffer, j ? 0b00100000 : 0b01100000, -1, (!style && idx) ? COLOR_WHITE : (field.secure ? COLOR_FONT_BOARD : (valid ? COLOR_FONT_PLAYER : COLOR_FONT_ERROR)), 0, 1, 0);
            }
        }
    }
    if (style)
    {
        for (int i = 1; i < 9; i++)
        {
            fill(OFFSET, i * SIZE, SIZE * 9, 1, COLOR_BORDER_LIGHT);
            fill(OFFSET + i * SIZE, 0, 1, SIZE * 9, COLOR_BORDER_LIGHT);
        }
    }
    for (int i = 1; i < 3; i++)
    {
        fill(OFFSET, i * 3 * SIZE - 1, SIZE * 9, 2, COLOR_BORDER_DARK);
        fill(OFFSET + i * 3 * SIZE - 1, 0, 2, SIZE * 9, COLOR_BORDER_DARK);
    }
}

#define FKEY_MENU 0x018C
#define FKEY_NEW 0x0186
#define FKEY_STYLE 0x0131
#define FKEY_RETRY 0x0114
#define FKEY_SOLVE 0x0148
#define FKEY_CLEAR 0x0149

void generate(Sudoku &sudoku, Sudoku &solution)
{
    SudokuGen temp;
    ProgressBar(0, 100);
    lasVegas(temp);
    solution = temp;
    ProgressBar(19, 100);
    dig(temp);
    sudoku = temp;
}

int main()
{
    int key, x = 4, y = 4;
    Sudoku sudoku, solution;
    bool style = true, menu = false;
    while (true)
    {
        EnableStatusArea(3);
        Bdisp_EnableColor(style);
        draw(sudoku, x, y, style);
        if (menu)
        {
            drawFKey(FKEY_NEW, 0);
            drawFKey(FKEY_STYLE, 1);
            drawFKey(FKEY_RETRY, 2);
            drawFKey(FKEY_SOLVE, 3);
            drawFKey(FKEY_CLEAR, 4);
        }
        else
        {
            drawFKey(FKEY_MENU, 0);
        }
        GetKey(&key);
        if (!menu && key == KEY_CTRL_F1)
        {
            menu = true;
        }
        else if (menu && key == KEY_CTRL_F1)
        {
            generate(sudoku, solution);
        }
        else if (menu && key == KEY_CTRL_F2)
        {
            style = !style;
        }
        else if (menu && key == KEY_CTRL_F3)
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (!sudoku.board[i][j].secure)
                    {
                        sudoku.board[i][j].value = EMPTY;
                    }
                }
            }
        }
        else if (menu && key == KEY_CTRL_F4)
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    sudoku.board[i][j].value = solution.board[i][j].value;
                }
            }
        }
        else if (menu && key == KEY_CTRL_F5)
        {
            memset(sudoku.board, EMPTY, sizeof(sudoku.board));
            solution = sudoku;
        }
        else if (menu && key == KEY_CTRL_EXIT)
        {
            menu = false;
        }
        else if (key == KEY_CTRL_UP && y > 0)
        {
            y--;
        }
        else if (key == KEY_CTRL_DOWN && y < 9 - 1)
        {
            y++;
        }
        else if (key == KEY_CTRL_LEFT && x > 0)
        {
            x--;
        }
        else if (key == KEY_CTRL_RIGHT && x < 9 - 1)
        {
            x++;
        }
        else if (key == KEY_CTRL_DEL)
        {
            sudoku.board[x][y].value = EMPTY;
        }
        else if (key >= KEY_CHAR_1 && key <= KEY_CHAR_9)
        {
            sudoku.board[x][y].value = key - KEY_CHAR_0;
        }
    }
    return 0;
}