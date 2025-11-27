#include "../include/lib.h"
#include <math.h>

// Реализация №1: Метод прямоугольников
float sin_integral(float a, float b, float e) {
    float integral = 0.0;
    for (float x = a; x < b; x += e) {
        integral += sin(x) * e;
    }
    return integral;
}

// Реализация №1: Формула (1 + 1/x)^x
float e(int x) {
    return pow(1.0 + 1.0 / x, x);
}