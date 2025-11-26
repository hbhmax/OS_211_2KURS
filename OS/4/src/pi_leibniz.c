#include "../include/pi_lib.h"

float pi(int k) {
    float sum = 0.0f;
    for (int i = 0; i < k; i++) {
        float term = 1.0f / (2 * i + 1);
        if (i % 2 == 1) {
            term = -term;
        }
        sum += term;
    }
    return 4.0f * sum;
}
