#include "../include/lib.h"
#include <math.h>

// Реализация №2: Метод трапеций
float sin_integral(float a, float b, float e) {
    float integral = 0.0;
    float n = (b - a) / e;
    for (int i = 0; i < n; i++) {
        float x1 = a + i * e;
        float x2 = a + (i + 1) * e;
        integral += (sin(x1) + sin(x2)) * e / 2;
    }
    return integral;
}

// Реализация №2: Сумма ряда
float e(int x) {
    float sum = 1.0;
    float factorial = 1.0;
    for (int i = 1; i <= x; i++) {
        factorial *= i;
        sum += 1.0 / factorial;
    }
    return sum;
}