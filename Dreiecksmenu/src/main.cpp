#include "converters.hpp"
#include "interaction.hpp"
#include "math.hpp"
#include "draw.hpp"
#include "bitmaps.hpp"

#include <fxcg/display.h>
#include <fxcg/keyboard.h>

#define FONT_HEIGHT 18
#define STATUS_AREA_HEIGHT 25

const char degree[1] = {-100};

const char *congruence[5] = {"3 Seiten", "2 Seiten 1 Winkel", "2 Winkel 1 Seite", "3 Punkte"};
const char *angle[3] = {"Alpha", "Beta", "Gamma"};
const char *side[3] = {"Seite a", "Seite b", "Seite c"};
const char *angles[3] = {"Alpha & Beta", "Alpha & Gamma", "Beta & Gamma"};
const char *sides[3] = {"Seite a & b", "Seite a & c", "Seite b & c"};
const char *dimensions[2] = {"2D", "3D"};

bool emptyValue;

void clearScreen()
{
    Bdisp_Fill_VRAM(COLOR_WHITE, 1);
}

void showCredits()
{
    int x = 0;
    int y = 0;
    PrintMini(&x, &y, "Version 1.4", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLUE, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Erstellt am 24.03.2021", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLUE, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT + 15;
    PrintMini(&x, &y, "Dieses Programm benutzt Taylor-", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "reihen zur Annaeherung der Wurzel", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "und trigonometrischer Funktionen.", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Es unterliegt daher Rundungsfehlern.", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Benutzen auf eigene Gefahr!", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_RED, 0, 1, 0);
    x = 0;
    y += FONT_HEIGHT + 15;
    PrintMini(&x, &y, "https://github.com/Pawl-Patrol", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    int key;
    GetKey(&key);
    clearScreen();
}

bool menu_three_sides(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, int *precision)
{
    int error = getValue(a, 1, 1, "  Seite a?", 50, precision);
    if (error)
        return error;
    error = getValue(b, 1, 3, "  Seite b?", 50, precision);
    if (error)
        return error;
    error = getValue(c, 1, 5, "  Seite c?", 50, precision);
    if (error)
        return error;
    three_sides(*a, *b, *c, alpha, beta, gamma);
    return false;
}

bool menu_two_sides(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, int *precision)
{
    int sel1 = getChoice("  Welche Seiten?", sides, 3);
    if (sel1 == -1)
        return true;
    int sel2 = getChoice("  Welcher Winkel?", angle, 3);
    if (sel1 == -1)
        return true;
    int error;
    if (sel1 == 0)
    {
        error = getValue(a, 1, 1, "  Seite a?", 50, precision);
        if (error)
            return error;
        error = getValue(b, 1, 3, "  Seite b?", 50, precision);
        if (error)
            return error;
    }
    else if (sel1 == 1)
    {
        error = getValue(a, 1, 1, "  Seite a?", 50, precision);
        if (error)
            return error;
        error = getValue(c, 1, 3, "  Seite c?", 50, precision);
        if (error)
            return error;
    }
    else if (sel1 == 2)
    {
        error = getValue(b, 1, 1, "  Seite b?", 50, precision);
        if (error)
            return error;
        error = getValue(c, 1, 3, "  Seite c?", 50, precision);
        if (error)
            return error;
    }
    if (sel2 == 0)
    {
        error = getValue(alpha, 1, 5, "  Alpha?", 50, precision);
        if (error)
            return error;
    }
    else if (sel2 == 1)
    {
        error = getValue(beta, 1, 5, "  Beta?", 50, precision);
        if (error)
            return error;
    }
    else if (sel2 == 2)
    {
        error = getValue(gamma, 1, 5, "  Gamma?", 50, precision);
        if (error)
            return error;
    }
    if (sel1 == 0)
    {
        if (sel2 == 0)
        {
            *beta = sin_angle(*alpha, *a, *b);
            *gamma = 180 - *alpha - *beta;
            *c = sin_side(*a, *alpha, *gamma);
        }
        else if (sel2 == 1)
        {
            *alpha = sin_angle(*beta, *b, *a);
            *gamma = 180 - *alpha - *beta;
            *c = sin_side(*b, *beta, *gamma);
        }
        else if (sel2 == 2)
        {
            *c = cos_side(*a, *b, *gamma);
            if (*a < *b)
            {
                *alpha = sin_angle(*gamma, *c, *a);
                *beta = 180 - *alpha - *gamma;
            }
            else
            {
                *beta = sin_angle(*gamma, *c, *b);
                *alpha = 180 - *beta - *gamma;
            }
        }
    }
    else if (sel1 == 1)
    {
        if (sel2 == 0)
        {
            *gamma = sin_angle(*alpha, *a, *c);
            *beta = 180 - *alpha - *gamma;
            *b = sin_side(*a, *alpha, *beta);
        }
        else if (sel2 == 1)
        {
            *b = cos_side(*a, *c, *beta);
            if (a < c)
            {
                *alpha = sin_angle(*beta, *b, *a);
                *gamma = 180 - *alpha - *beta;
            }
            else
            {
                *gamma = sin_angle(*beta, *b, *c);
                *alpha = 180 - *beta - *gamma;
            }
        }
        else if (sel2 == 2)
        {
            *alpha = sin_angle(*gamma, *c, *a);
            *beta = 180 - *alpha - *gamma;
            *b = sin_side(*c, *gamma, *beta);
        }
    }
    else if (sel1 == 2)
    {
        if (sel2 == 0)
        {
            *a = cos_side(*b, *c, *alpha);
            if (b < c)
            {
                *beta = sin_angle(*alpha, *a, *b);
                *gamma = 180 - *alpha - *beta;
            }
            else
            {
                *gamma = sin_angle(*alpha, *a, *c);
                *beta = 180 - *alpha - *gamma;
            }
        }
        else if (sel2 == 1)
        {
            *gamma = sin_angle(*beta, *b, *c);
            *alpha = 180 - *beta - *gamma;
            *a = sin_side(*b, *beta, *alpha);
        }
        else if (sel2 == 2)
        {
            *alpha = sin_angle(*gamma, *c, *a);
            *beta = 180 - *alpha - *gamma;
            *a = sin_side(*c, *gamma, *alpha);
        }
    }
    return false;
}

bool menu_two_angles(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, int *precision)
{
    int sel1 = getChoice("  Welche Winkel?", angles, 3);
    if (sel1 == -1)
        return true;
    int sel2 = getChoice("  Welche Seite?", side, 3);
    if (sel2 == -1)
        return true;

    int error;
    if (sel1 == 0)
    {
        error = getValue(alpha, 1, 1, "  Alpha?", 50, precision);
        if (error)
            return error;
        error = getValue(beta, 1, 3, "  Beta?", 50, precision);
        if (error)
            return error;
        *gamma = 180 - *alpha - *beta;
    }
    else if (sel1 == 1)
    {
        error = getValue(alpha, 1, 1, "  Alpha?", 50, precision);
        if (error)
            return error;
        error = getValue(gamma, 1, 3, "  Gamma?", 50, precision);
        if (error)
            return error;
        *beta = 180 - *alpha - *gamma;
    }
    else if (sel1 == 2)
    {
        error = getValue(beta, 1, 1, "  Beta?", 50, precision);
        if (error)
            return error;
        error = getValue(gamma, 1, 3, "  Gamma?", 50, precision);
        if (error)
            return error;
        *alpha = 180 - *beta - *gamma;
    }
    if (sel2 == 0)
    {
        error = getValue(a, 1, 5, "  Seite a?", 50, precision);
        if (error)
            return error;
    }
    else if (sel2 == 1)
    {
        error = getValue(b, 1, 5, "  Seite b?", 50, precision);
        if (error)
            return error;
    }
    else if (sel2 == 2)
    {
        error = getValue(c, 1, 5, "  Seite c?", 50, precision);
        if (error)
            return error;
    }
    if (sel2 == 0)
    {
        *b = sin_side(*a, *alpha, *beta);
        *c = sin_side(*a, *alpha, *gamma);
    }
    else if (sel2 == 1)
    {
        *a = sin_side(*b, *beta, *alpha);
        *c = sin_side(*b, *beta, *gamma);
    }
    else if (sel2 == 2)
    {
        *a = sin_side(*c, *gamma, *alpha);
        *b = sin_side(*c, *gamma, *beta);
    }
    return false;
}

bool menu_three_points(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, int *precision)
{
    int sel = getChoice("  Dimensionen?", dimensions, 2);
    if (sel == -1)
        return true;
    float ax, ay, az, bx, by, bz, cx, cy, cz;
    int error = getValue(&ax, 1, 1, "  x von A?", 50, precision);
    if (error)
        return error;
    error = getValue(&ay, 1, 3, "  y von A?", 50, precision);
    if (error)
        return error;
    if (sel)
    {
        error = getValue(&az, 1, 5, "  z von A?", 50, precision);
        if (error)
            return error;
    }
    clearScreen();
    error = getValue(&bx, 1, 1, "  x von B?", 50, precision);
    if (error)
        return error;
    error = getValue(&by, 1, 3, "  y von B?", 50, precision);
    if (error)
        return error;
    if (sel)
    {
        error = getValue(&bz, 1, 5, "  z von B?", 50, precision);
        if (error)
            return error;
    }
    clearScreen();
    error = getValue(&cx, 1, 1, "  x von C?", 50, precision);
    if (error)
        return error;
    error = getValue(&cy, 1, 3, "  y von C?", 50, precision);
    if (error)
        return error;
    if (sel)
    {
        error = getValue(&cz, 1, 5, "  z von C?", 50, precision);
        if (error)
            return error;
    }
    *a = bx - cx;
    *a *= *a;
    float buf = by - cy;
    buf *= buf;
    *a += buf;
    if (sel)
    {
        buf = bz - cz;
        buf *= buf;
        *a += buf;
    }
    *a = sqrt(*a);
    *b = ax - cx;
    *b *= *b;
    buf = ay - cy;
    buf *= buf;
    *b += buf;
    if (sel)
    {
        buf = az - cz;
        buf *= buf;
        *b += buf;
    }
    *b = sqrt(*b);
    *c = ax - bx;
    *c *= *c;
    buf = ay - by;
    buf *= buf;
    *c += buf;
    if (sel)
    {
        buf = az - bz;
        buf *= buf;
        *c += buf;
    }
    *c = sqrt(*c);
    three_sides(*a, *b, *c, alpha, beta, gamma);
    return false;
}

void drawFKeys()
{
    drawFKey(0, sss);
    drawFKey(1, ssw);
    drawFKey(2, wws);
    drawFKey(3, ppp);
}

int calculate(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, float *u, float *area, float *ha, float *hb, float *hc, float *ir, float *ur)
{
    if (*a <= 0 || *b <= 0 || *c <= 0 || *alpha <= 0 || *beta <= 0 || *gamma <= 0)
    {
        return 80;
    }
    *u = *a + *b + *c;
    float s = *u / 2;
    float ralpha = rad(*alpha);
    float rbeta = rad(*beta);
    float rgamma = rad(*gamma);
    float sinalpha = sin(ralpha);
    float sinbeta = sin(rbeta);
    float singamma = sin(rgamma);
    *ha = *b * singamma;
    *ha += *c * sinbeta;
    *ha /= 2.0f;
    *hb = *c * sinalpha;
    *hb += *a * singamma;
    *hb /= 2.0f;
    *hc = *a * sinbeta;
    *hc += *b * sinalpha;
    *hc /= 2.0f;

    *ir = (s - *a) * sin(0.5f * ralpha) / cos(0.5f * ralpha);
    *ir += (s - *b) * sin(0.5f * rbeta) / cos(0.5f * rbeta);
    *ir += (s - *c) * sin(0.5f * rgamma) / cos(0.5f * rgamma);
    *ir /= 3.0f;

    *ur = *a / (2 * sinalpha);
    *ur += *b / (2 * sinbeta);
    *ur += *c / (2 * singamma);
    *ur /= 3.0f;

    *area = 0.5f * *a;
    *area *= *ha;
    float area2 = 0.5f * *b;
    area2 *= *hb;
    *area += area2;
    float area3 = 0.5f * *c;
    area3 *= *hc;
    *area += area3;
    float area4 = *ur;
    area4 *= *ur;
    area4 *= 2.0f * sinalpha * sinbeta * singamma;
    *area += area4;
    *area /= 4.0f;
    return 0;
}

void printFloat(int *x, int *y, float f, int maxlen, int precision)
{
    PrintMini(x, y, ftoa(f, precision, precision + 3), 0x02, maxlen, 0, 0, COLOR_BLACK, 0, 1, 0);
}

void print_result(float *a, float *b, float *c, float *alpha, float *beta, float *gamma, float *u, float *area, float *ha, float *hb, float *hc, float *ir, float *ur, int precision)
{
    Bdisp_EnableColor(1);
    int w2 = LCD_WIDTH_PX / 2;
    int x = 0, y = 0;
    PrintMini(&x, &y, "Seite a: ", 0x02, w2, 0, 0, COLOR_RED, 0, 1, 0);
    printFloat(&x, &y, *a, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Seite b: ", 0x02, w2, 0, 0, COLOR_BLUE, 0, 1, 0);
    printFloat(&x, &y, *b, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Seite c: ", 0x02, w2, 0, 0, COLOR_GREEN, 0, 1, 0);
    printFloat(&x, &y, *c, w2, precision);
    x = 0;
    y += FONT_HEIGHT + 5;
    PrintMini(&x, &y, "Hoehe a: ", 0x02, w2, 0, 0, COLOR_MAROON, 0, 1, 0);
    printFloat(&x, &y, *ha, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Hoehe b: ", 0x02, w2, 0, 0, COLOR_MAROON, 0, 1, 0);
    printFloat(&x, &y, *hb, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Hoehe c: ", 0x02, w2, 0, 0, COLOR_MAROON, 0, 1, 0);
    printFloat(&x, &y, *hc, w2, precision);
    x = 0;
    y += FONT_HEIGHT + 5;
    PrintMini(&x, &y, "Umfang: ", 0x02, w2, 0, 0, COLOR_DARKCYAN, 0, 1, 0);
    printFloat(&x, &y, *u, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Flaeche: ", 0x02, w2, 0, 0, COLOR_DARKCYAN, 0, 1, 0);
    printFloat(&x, &y, *area, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Inkreis: ", 0x02, w2, 0, 0, COLOR_DARKCYAN, 0, 1, 0);
    printFloat(&x, &y, *ir, w2, precision);
    x = 0;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Umkreis: ", 0x02, w2, 0, 0, COLOR_DARKCYAN, 0, 1, 0);
    printFloat(&x, &y, *ur, w2, precision);
    x = w2;
    y = 0;
    PrintMini(&x, &y, "Alpha: ", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_INDIGO, 0, 1, 0);
    printFloat(&x, &y, *alpha, LCD_WIDTH_PX, precision);
    PrintMini(&x, &y, degree, 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = w2;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Beta: ", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_INDIGO, 0, 1, 0);
    printFloat(&x, &y, *beta, LCD_WIDTH_PX, precision);
    PrintMini(&x, &y, degree, 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = w2;
    y += FONT_HEIGHT;
    PrintMini(&x, &y, "Gamma: ", 0x02, LCD_WIDTH_PX, 0, 0, COLOR_INDIGO, 0, 1, 0);
    printFloat(&x, &y, *gamma, LCD_WIDTH_PX, precision);
    PrintMini(&x, &y, degree, 0x02, LCD_WIDTH_PX, 0, 0, COLOR_BLACK, 0, 1, 0);
    x = w2;
    y += FONT_HEIGHT;
    int h = LCD_HEIGHT_PX - STATUS_AREA_HEIGHT - y;
    float rel = h / (*ur * 2.0);
    float crel = rel * *c;
    float hrel = rel * *hc;
    float dx = cos(rad(*alpha)) * *b * rel;
    x += (w2 - crel) / 2.0;
    y = LCD_HEIGHT_PX - (h - hrel) / 2.0;
    plotLineAA(x + crel, y, x + dx, y - hrel, COLOR_RED);
    plotLineAA(x, y, x + dx, y - hrel, COLOR_BLUE);
    plotLineAA(x, y, x + crel, y, COLOR_GREEN);
}

int main_loop()
{
    float a, b, c, alpha, beta, gamma;
    int error, precision = 0;
    switch (getChoice("  Dreiecksberechner", congruence, 4))
    {
    case 0:
        error = menu_three_sides(&a, &b, &c, &alpha, &beta, &gamma, &precision);
        break;
    case 1:
        error = menu_two_sides(&a, &b, &c, &alpha, &beta, &gamma, &precision);
        break;
    case 2:
        error = menu_two_angles(&a, &b, &c, &alpha, &beta, &gamma, &precision);
        break;
    case 3:
        error = menu_three_points(&a, &b, &c, &alpha, &beta, &gamma, &precision);
        break;
    default:
        showCredits();
        error = 1;
    }
    Cursor_SetFlashOff();
    if (error) return error;
    float u, area, ha, hb, hc, ir, ur;
    error = calculate(&a, &b, &c, &alpha, &beta, &gamma, &u, &area, &ha, &hb, &hc, &ir, &ur);
    if (error)
        return error;
    clearScreen();
    Bdisp_EnableColor(1);
    print_result(&a, &b, &c, &alpha, &beta, &gamma, &u, &area, &ha, &hb, &hc, &ir, &ur, precision + 1);
    int key;
    GetKey(&key);
    Bdisp_EnableColor(0);
    return 0;
}

void print_error_msg(int error)
{
    PrintXY_2(TEXT_MODE_NORMAL, 2, 6, error, TEXT_COLOR_RED);
}

int main()
{
    int error;
    while (true)
    {
        drawFKeys();
        error = main_loop();
        clearScreen();
        if (error > 1)
        {
            print_error_msg(error);
        }
    }
    return 0;
}
