#include "../include/pi_lib.h"

float pi(int k) {
    float product = 1.0f;
    for (int n = 1; n <= k; n++) {
        float numerator = (2 * n) * (2 * n);
        float denominator = (2 * n - 1) * (2 * n + 1);
        product *= numerator / denominator;
    }
    return 2.0f * product;
}
