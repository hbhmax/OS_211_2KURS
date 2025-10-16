#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

static char SERVER_PROGRAM_NAME[] = "server";

int main(int argc, char **argv) {
    if (argc != 3) {
        char msg[1024];
        uint32_t len = snprintf(msg, sizeof(msg) - 1, "usage: %s filename1 filename2\n", argv[0]);
        write(STDERR_FILENO, msg, len);
        exit(EXIT_SUCCESS);
    }

    srand(time(NULL)); // для вероятностного распределения

    // NOTE: Get full path to the directory, where program resides
    char progpath[1024];
    {
        ssize_t len = readlink("/proc/self/exe", progpath, sizeof(progpath) - 1);
        if (len == -1) {
            const char msg[] = "error: failed to read full program path\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }

        while (progpath[len] != '/')
            --len;
        progpath[len] = '\0';
    }

    // NOTE: Open pipes for TWO children
    int pipe1[2]; // для первого ребенка
    if (pipe(pipe1) == -1) {
        const char msg[] = "error: failed to create pipe1\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    int pipe2[2]; // для второго ребенка
    if (pipe(pipe2) == -1) {
        const char msg[] = "error: failed to create pipe2\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    // NOTE: Spawn FIRST child process
    const pid_t child1 = fork();
    switch (child1) {
    case -1: {
        const char msg[] = "error: failed to spawn first child process\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    } break;

    case 0: { // FIRST child
        close(pipe1[1]); // закрываем запись
        dup2(pipe1[0], STDIN_FILENO); // stdin читает из pipe1
        close(pipe1[0]);

        {
            char path[1024];
            snprintf(path, sizeof(path) - 1, "%s/%s", progpath, SERVER_PROGRAM_NAME);
            char *const args[] = {SERVER_PROGRAM_NAME, argv[1], NULL}; // argv[1] - первый файл
            int32_t status = execv(path, args);

            if (status == -1) {
                const char msg[] = "error: failed to exec into server\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                exit(EXIT_FAILURE);
            }
        }
    } break;

    default: { // Parent continues
        // NOTE: Spawn SECOND child process
        const pid_t child2 = fork();
        switch (child2) {
        case -1: {
            const char msg[] = "error: failed to spawn second child process\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        } break;

        case 0: { // SECOND child
            close(pipe2[1]); // закрываем запись
            dup2(pipe2[0], STDIN_FILENO); // stdin читает из pipe2
            close(pipe2[0]);

            {
                char path[1024];
                snprintf(path, sizeof(path) - 1, "%s/%s", progpath, SERVER_PROGRAM_NAME);
                char *const args[] = {SERVER_PROGRAM_NAME, argv[2], NULL}; // argv[2] - второй файл
                int32_t status = execv(path, args);

                if (status == -1) {
                    const char msg[] = "error: failed to exec into server\n";
                    write(STDERR_FILENO, msg, sizeof(msg));
                    exit(EXIT_FAILURE);
                }
            }
        } break;

        default: { // Parent process
            close(pipe1[0]); // закрываем чтение
            close(pipe2[0]); // закрываем чтение

            char buf[4096];
            ssize_t bytes;

            printf("Input string:\n");
            
            while ((bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
                if (bytes < 0) {
                    const char msg[] = "error: failed to read from stdin\n";
                    write(STDERR_FILENO, msg, sizeof(msg));
                    exit(EXIT_FAILURE);
                } else if (buf[0] == '\n') {
                    break; // завершение по пустой строке
                }

                // ВЕРОЯТНОСТНОЕ РАСПРЕДЕЛЕНИЕ: 80% vs 20%
                int r = rand() % 100;
                if (r < 80) {
                    write(pipe1[1], buf, bytes); // 80% - в pipe1
                } else {
                    write(pipe2[1], buf, bytes); // 20% - в pipe2
                }
            }

            close(pipe1[1]);
            close(pipe2[1]);
        } break;
        }
    } break;
    }

    return 0;
}