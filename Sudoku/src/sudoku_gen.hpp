#ifndef SUDOKU_GEN_HPP
#define SUDOKU_GEN_HPP

#include <fxcg/rtc.h>

#include "sudoku.hpp"

class SudokuGen : public Sudoku
{
public:
    int index;
    char getAtIndex()
    {
        return board[index % 9][index / 9].value;
    }
    void setAtIndex(char value)
    {
        board[index % 9][index / 9].value = value;
    }
    void toggleIndex(bool value)
    {
        board[index % 9][index / 9].secure = value;
    }
    bool isIndexLast()
    {
        for (int i = index; i < 81; i++)
        {
            if (board[i % 9][i / 9].value == EMPTY)
            {
                return false;
            }
        }
        return true;
    }
    void moveIndex()
    {
        while (getAtIndex() != EMPTY && !isIndexLast())
        {
            index++;
        }
    }
    void resetIndex()
    {
        index = 0;
        moveIndex();
    }
    bool solve(int start, int timeout)
    {
        if (RTC_Elapsed_ms(start, timeout))
        {
            return false;
        }
        moveIndex();
        if (isIndexLast())
        {
            return true;
        }
        int temp = index;
        for (int i = 0; i < 9; i++)
        {
            if (isPossible(index % 9, index / 9, i + 1))
            {
                setAtIndex(i + 1);
                if (solve(start, timeout))
                {
                    return true;
                }
                index = temp;
                setAtIndex(EMPTY);
            }
        }
        return false;
    }
    void countSolutions(int &counter, int max)
    {
        moveIndex();
        if (isIndexLast())
        {
            counter++;
            return;
        }
        int temp = index;
        for (int i = 0; i < 9 && counter < max; i++)
        {
            if (isPossible(index % 9, index / 9, i + 1))
            {
                setAtIndex(i + 1);
                countSolutions(counter, max);
                index = temp;
                setAtIndex(EMPTY);
            }
        }
    }
    int countNums()
    {
        int total = 0;
        for (int x = 0; x < 9; x++)
        {
            for (int y = 0; y < 9; y++)
            {
                total += board[x][y].value != EMPTY;
            }
        }
        return total;
    }
};

#endif