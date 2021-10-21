#include "converters.hpp"

#define EXPONENT_SYMBOL 15

float atof(char *str, int *precision)
{
    float result = 0, fact = 1;
    if (*str == '-')
    {
        str++;
        fact = -1;
    }
    int local_precision = 0;
    for (bool point = false; *str; str++)
    {
        if (*str == '.')
        {
            point = true;
            continue;
        }
        int digit = *str - '0';
        if (digit >= 0 && digit <= 9)
        {
            if (point)
            {
                fact /= 10.0f;
                local_precision++;
                if (local_precision > *precision)
                {
                    *precision = local_precision;
                }
            }
            result = result * 10.0f + (float)digit;
        }
    }
    return result * fact;
}

float round(float value, int digits)
{
    float mult = 1;
    while (digits--) mult *= 10;
    return (float)(int)(value * mult + .5) / mult;
}

char *ftoa(float num, int precision, int maxlen)
{
    num = round(num, precision);
    unsigned long long f = 1000000000000LL;
    unsigned long long fraction = 0;
    unsigned long long digits = 0;
    unsigned int data = *((int *)(&num));
    signed char e = data >> 23;
    e &= 0xff;
    e -= 127;
    bool sign = data & 0b10000000000000000000000000000000;
    unsigned int sig = data & 0b00000000011111111111111111111111;
    sig |= 0b00000000100000000000000000000000;
    sig <<= 8;
    for (int i = 23; i; i--)
    {
        bool a = sig & 0b10000000000000000000000000000000;
        if (a)
        {
            unsigned long long ft = f, dt = 0;
            for (int ei = e; ei != 0; ei -= (ei >= 0 ? 1 : -1))
            {
                if (ei > 0)
                {
                    ft <<= 1;
                    dt <<= 1;
                }
                else
                {
                    ft >>= 1;
                    dt >>= 1;
                }

                while (ft >= 1000000000000LL)
                {
                    ft -= 1000000000000LL;
                    dt++;
                }
            }
            digits += dt;
            fraction += ft;

            while (fraction >= 1000000000000LL)
            {
                fraction -= 1000000000000LL;
                digits++;
            }
        }

        f >>= 1;
        sig <<= 1;
    }
    char *rbuffer = (char *)sys_malloc(sizeof(char) * 8 + precision);
    char *rptr = rbuffer;
    if (sign)
    {
        *rptr++ = '-';
    }
    char dbuffer[21];
    char *dptr = dbuffer;
    char *reversedbuffer = (char *)sys_malloc(sizeof(char) * 21);
    *reversedbuffer = '\0';
    while (digits)
    {
        *++reversedbuffer = '0' + digits % 10;
        digits /= 10;
    }
    int len = 0;
    while (*reversedbuffer)
    {
        *dptr++ = *reversedbuffer--;
        len++;
    }
    *dptr = '\0';
    sys_free(reversedbuffer);
    if (len >= maxlen)
    {
        *rptr++ = dbuffer[0];
        *rptr++ = '.';
        for (int i = 0; i < precision && i < len - 1; i++)
        {
            *rptr++ = dbuffer[i + 1];
        }
        *rptr++ = EXPONENT_SYMBOL;
        itoa(len - 1, (unsigned char *)rptr);
    }
    else
    {
        if (len)
        {
            for (int i = 0; i < len; i++)
            {
                *rptr++ = dbuffer[i];
            }
        }
        else
        {
            *rptr++ = '0';
        }
        *rptr = '.';
        char fbuffer[12];
        char *fptr = fbuffer + 11;
        for (int i = 11; i >= 0; i--)
        {
            *fptr-- = '0' + fraction % 10;
            fraction /= 10;
        }
        rptr += precision;
        bool can_skip = true;
        for (int i = precision - 1; i >= 0; i--)
        {
            if (can_skip)
            {
                if (fbuffer[i] == '0')
                {
                    rptr--;
                    continue;
                }
                *(rptr + 1) = '\0';
                can_skip = false;
            }
            *rptr-- = fbuffer[i];
        }
        if (can_skip)
        {
            *rptr = '\0';
        }
    }
    return rbuffer;
}