#include "interaction.hpp"

int getChoice(const char *title, const char **choices, int length)
{
    PrintXY(1, 1, title, TEXT_MODE_NORMAL, TEXT_COLOR_BLUE);
    int index = 0;
    locate_OS(2, 2);
    Print_OS(choices[0], TEXT_MODE_INVERT, 0);
    for (int i = 1; i < length; i++)
    {
        locate_OS(2, 2 + i);
        Print_OS(choices[i], TEXT_MODE_NORMAL, 0);
    }
    int key;
    while (true)
    {
        GetKey(&key);
        if (key == KEY_CTRL_DOWN || key == KEY_CTRL_UP)
        {
            locate_OS(2, 2 + index);
            Print_OS(choices[index], TEXT_MODE_NORMAL, 0);
            index = (index + (key == KEY_CTRL_DOWN ? 1 : -1) + length) % length;
            locate_OS(2, 2 + index);
            Print_OS(choices[index], TEXT_MODE_INVERT, 0);
        }
        else if (key >= KEY_CTRL_F1 && key <= KEY_CTRL_F6)
        {
            index = key - KEY_CTRL_F1;
            break;
        }
        else if (key == KEY_CTRL_EXE)
        {
            break;
        }
        else if (key == KEY_CTRL_EXIT)
        {
            index = -1;
            break;
        }
    }
    Bdisp_Fill_VRAM(COLOR_WHITE, 1);
    return index;
}

bool getValue(float *value, int x, int y, const char *msg, int maxlen, int *precision)
{
    PrintXY(x, y, msg, TEXT_MODE_NORMAL, TEXT_COLOR_BLUE);
    y++;
    int start = 0, cursor = 0;
    char *buffer = (char *)sys_malloc(sizeof(char) * maxlen);
    *buffer = '\0';
    DisplayMBString((unsigned char *)buffer, start, cursor, x, y);
    int key;
    while (true)
    {
        GetKey(&key);
        if (key == KEY_CTRL_EXE)
        {
            break;
        }
        else if (key == KEY_CTRL_EXIT)
        {
            return true;
        }
        else if ((key >= KEY_CHAR_0 && key <= KEY_CHAR_9) || key == KEY_CHAR_DP)
        {
            cursor = EditMBStringChar((unsigned char *)buffer, maxlen, cursor, key);
            DisplayMBString((unsigned char *)buffer, start, cursor, x, y);
        }
        else
        {
            EditMBStringCtrl((unsigned char *)buffer, maxlen, &start, &cursor, &key, x, y);
        }
    }
    *value = atof(buffer, precision);
    return false;
}
