#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int is_vowel(char c) {
    c = tolower(c);
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y');
}

ssize_t remove_vowels(char *str, ssize_t length) {
    char *src = str;
    char *dst = str;
    ssize_t res_len = length;
    
    for (ssize_t i = 0; i < length; i++) {
        if (!is_vowel(*src)) {
            *dst++ = *src;
        }
        else {
            res_len--;
        }
        src++;
    }

    return res_len;
}

int main(int argc, char **argv) {
    char buf[4096];
    ssize_t bytes;

    pid_t pid = getpid();

    int32_t file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (file == -1) {
        const char msg[] = "error: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    while ((bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (bytes < 0) {
            const char msg[] = "error: failed to read from stdin\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }

        ssize_t res_bytes = remove_vowels(buf, bytes);

        write(file, buf, res_bytes);
    }
    close(file);
    return 0;
}