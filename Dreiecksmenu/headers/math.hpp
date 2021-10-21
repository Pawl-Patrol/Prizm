#define TRIG_ITERATIONS 10
#define SQRT_ITERATIONS 15

const float PI = 3.141592653589793;

float max(float a, float b);
int abs(int a);
float rad(float deg);
float deg(float rad);
float sqrt(float x);
int factorial(int x);
float sin(float x);
float cos(float x);
float arcsin(float x);
float arccos(float x);
float cos_side(float side1, float side2, float opposite_angle);
float cos_angle(float side1, float side2, float opposite_side);
float sin_side(float side, float opposite_angle, float angle);
float sin_angle(float opposite_angle, float side, float opposite_side);
void three_sides(float a, float b, float c, float *alpha, float *beta, float *gamma);
