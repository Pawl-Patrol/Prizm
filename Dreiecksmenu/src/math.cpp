#include "math.hpp"

float max(float a, float b)
{
    return a > b ? a : b;
}

int abs(int a)
{
    return a > -a ? a : -a;
}

float rad(float deg)
{
    return deg * PI / 180;
}

float deg(float rad)
{
    return rad * 180 / PI;
}

float sqrt(float x)
{
    if (x == 0)
    {
        return 0;
    }
    float a, b, guess = x;
    for (int i = 0; i < SQRT_ITERATIONS; i++)
    {
        a = guess;
        a *= a;
        a -= x;
        b = 2;
        b *= guess;
        a /= b;
        guess -= a;
    }
    return guess;
}

int factorial(int x)
{
    if (x == 0)
    {
        return 1;
    }
    else
    {
        return x * factorial(x - 1);
    }
}

float sin(float x)
{
    if (x == 0.0f)
    {
        return 0;
    }
    float x_sq = x;
    char sign = 1;
    float result = x;
    float a;
    x *= x;
    for (int n = 1; n < TRIG_ITERATIONS; n++)
    {
        x_sq *= x;
        sign *= -1;
        a = sign * x_sq;
        a /= (float)factorial(2 * n + 1);
        result += a;
    }
    return result;
}

float cos(float x)
{
    if (x == 0.0)
    {
        return 1;
    }
    x *= x;
    float x_sq = 1;
    char sign = 1;
    float result = 1;
    float a;
    for (int n = 1; n < TRIG_ITERATIONS; n++)
    {
        x_sq *= x;
        sign *= -1;
        a = sign * x_sq;
        a /= (float)factorial(2 * n);
        result += a;
    }
    return result;
}

float arcsin(float x)
{
    if (x == 0.0)
    {
        return 0;
    }
    float result = x;
    float mfour = 1;
    float fact1 = 1;
    float mult = x;
    float a, b;
    x *= x;
    for (int n = 1; n < TRIG_ITERATIONS; n++)
    {
        mfour *= 4;
        fact1 *= n;
        mult *= x;
        a = (float)factorial(2 * n);
        a *= mult;
        b = mfour;
        b *= fact1;
        b *= fact1;
        b *= 2 * n + 1;
        a /= b;
        result += a;
    }
    return result;
}

float arccos(float x)
{
    if (x == 1.0)
    {
        return 0;
    }
    return PI / 2 - arcsin(x);
}

float cos_side(float side1, float side2, float opposite_angle)
{
    float x = side1;
    x *= side2;
    x *= -2 * cos(rad(opposite_angle));
    side1 *= side1;
    side2 *= side2;
    x += side1;
    x += side2;
    return sqrt(x);
}

float cos_angle(float side1, float side2, float opposite_side)
{
    float x = 2;
    x *= side1;
    x *= side2;
    side1 *= side1;
    side2 *= side2;
    opposite_side *= opposite_side;
    side1 += side2;
    side1 -= opposite_side;
    side1 /= x;
    return deg(arccos(side1));
}

float sin_side(float side, float opposite_angle, float angle)
{
    return sin(rad(angle))*side/sin(rad(opposite_angle));
}

float sin_angle(float opposite_angle, float side, float opposite_side)
{
    return deg(arcsin(opposite_side*sin(rad(opposite_angle))/side));
}

void three_sides(float a, float b, float c, float *alpha, float *beta, float *gamma)
{
    *alpha = cos_angle(b, c, a);
    *beta = cos_angle(a, c, b);
    *gamma = 180 - *alpha - *beta;
}
