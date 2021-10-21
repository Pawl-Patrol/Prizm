#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include "string.h"

#define EMPTY 0
#define idx(x, y) y * 9 + x

struct Field
{
    unsigned char value : 4;
    bool secure : 1;
};

class Sudoku
{
    public:
        Field board[9][9] = {EMPTY};
        bool isPossible(int x, int y, char value)
        {
            for (int i = 0; i < 9; i++)
            {
                if (i != x && board[i][y].value == value)
                {
                    return false;
                }
                if (i != y && board[x][i].value == value)
                {
                    return false;
                }
                if (!(x == x / 3 * 3 + i % 3 && y == y / 3 * 3 + i / 3) && board[x / 3 * 3 + i % 3][y / 3 * 3 + i / 3].value == value)
                {
                    return false;
                }
            }
            return true;
        }
};

#endif