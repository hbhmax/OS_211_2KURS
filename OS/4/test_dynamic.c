#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

typedef float (*pi_func_t)(int);

int main(int argc, char *argv[]) {
    pi_func_t pi_func = NULL;
    void *handle = NULL;

    const char *default_lib = "./libpi_leibniz.so";
    const char *other_lib = "./libpi_wallis.so";

    handle = dlopen(default_lib, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки %s: %s\n", default_lib, dlerror());
        return 1;
    }
    pi_func = (pi_func_t)dlsym(handle, "pi");
    if (!pi_func) {
        fprintf(stderr, "Ошибка: символ 'pi' не найден в библиотеке\n");
        dlclose(handle);
        return 1;
    }

    char command[100];
    int k;

    while (1) {
        printf("Команда (0 — выйти, 1 k — вычислить pi(k), 2 - сменить библиотеку): ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        if (command[0] == '2') {
            dlclose(handle);

            if (strcmp(default_lib, "./libpi_leibniz.so") == 0) {
                default_lib = other_lib;
            } else {
                default_lib = "./libpi_leibniz.so";
            }

            handle = dlopen(default_lib, RTLD_LAZY);
            if (!handle) {
                fprintf(stderr, "Ошибка загрузки %s: %s\n", default_lib, dlerror());
                handle = dlopen("./libpi_leibniz.so", RTLD_LAZY);
                if (!handle) return 1;
            }
            pi_func = (pi_func_t)dlsym(handle, "pi");
            if (!pi_func) {
                fprintf(stderr, "Ошибка: символ 'pi' не найден\n");
                dlclose(handle);
                return 1;
            }
            printf("Переключено на библиотеку: %s\n", default_lib);
        } else if (command[0] == '1') {
            if (sscanf(command + 2, "%d", &k) == 1 && k > 0) {
                float result = pi_func(k);
                printf("pi(%d) = %.6f (библиотека: %s)\n", k, result, default_lib);
            } else {
                printf("Ошибка: неверный формат аргумента\n");
            }
        } else if (command[0] == '0') {
            break;
        } else {
            printf("Неизвестная команда\n");
        }
    }

    if (handle) {
        dlclose(handle);
    }
    return 0;
}
