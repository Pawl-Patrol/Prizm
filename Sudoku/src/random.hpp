#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <fxcg/rtc.h>

int RandInt(int lower, int upper)
{
    int a = 123456789;
    int b = 362436069;
    int c = 521288629;
    int d = RTC_GetTicks();
    int t;

    t = a ^ (a << 11);
    a = b;
    b = c;
    c = d;
    return (d = d ^ (d >> 19) ^ (t ^ (t >> 8))) % upper + lower;
}

void shuffle(char *ptr, int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = RandInt(0, i + 1);
        char temp = ptr[i];
        ptr[i] = ptr[j];
        ptr[j] = temp;
    }
}

#endif