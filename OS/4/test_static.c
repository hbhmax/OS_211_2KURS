#include <stdio.h>
#include <stdlib.h>
#include "include/pi_lib.h"

int main() {
    char command[100];
    int k;

    while (1) {
        printf("Команда (0 — выйти, 1 k — вычислить pi(k)): ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        if (command[0] == '0') {
            break;
        } else if (command[0] == '1') {
            if (sscanf(command + 2, "%d", &k) == 1 && k > 0) {
                float result = pi(k);
                printf("pi(%d) = %.6f\n", k, result);
            } else {
                printf("Ошибка: неверный формат аргумента\n");
            }
        } else {
            printf("Неизвестная команда\n");
        }
    }

    return 0;
}
