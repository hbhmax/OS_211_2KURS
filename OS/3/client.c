#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <wait.h>
#include <semaphore.h>
#include <sys/mman.h>

#define SHM_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc != 3) {
        const char msg[] = "usage: client filename1 filename2\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    char shm_name1[256], shm_name2[256];
    snprintf(shm_name1, sizeof(shm_name1), "/shm_%d_1", getpid());
    snprintf(shm_name2, sizeof(shm_name2), "/shm_%d_2", getpid());

    int shm1 = shm_open(shm_name1, O_RDWR | O_CREAT | O_TRUNC, 0600);
    int shm2 = shm_open(shm_name2, O_RDWR | O_CREAT | O_TRUNC, 0600);
    
    if (shm1 == -1 || shm2 == -1) {
        const char msg[] = "error: failed to create SHM\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    if (ftruncate(shm1, SHM_SIZE) == -1 || ftruncate(shm2, SHM_SIZE) == -1) {
        const char msg[] = "error: failed to resize SHM\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    char *shm_buf1 = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0);
    char *shm_buf2 = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm2, 0);
    
    if (shm_buf1 == MAP_FAILED || shm_buf2 == MAP_FAILED) {
        const char msg[] = "error: failed to map SHM\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    char sem_name1[256], sem_name2[256];
    snprintf(sem_name1, sizeof(sem_name1), "/sem_%d_1", getpid());
    snprintf(sem_name2, sizeof(sem_name2), "/sem_%d_2", getpid());

    sem_t *sem1 = sem_open(sem_name1, O_CREAT, 0600, 1);
    sem_t *sem2 = sem_open(sem_name2, O_CREAT, 0600, 1);
    
    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED) {
        const char msg[] = "error: failed to create semaphore\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    pid_t server1 = fork();
    if (server1 == 0) {
        char *args[] = {"server", argv[1], shm_name1, sem_name1, NULL};
        execv("./server", args);
        
        const char msg[] = "error: failed to exec server 1\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    } else if (server1 == -1) {
        const char msg[] = "error: failed to fork server 1\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    pid_t server2 = fork();
    if (server2 == 0) {
        char *args[] = {"server", argv[2], shm_name2, sem_name2, NULL};
        execv("./server", args);
        
        const char msg[] = "error: failed to exec server 2\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    } else if (server2 == -1) {
        const char msg[] = "error: failed to fork server 2\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        _exit(EXIT_FAILURE);
    }

    bool running = true;
    while (running) {
        char buf[SHM_SIZE - sizeof(uint32_t)];
        ssize_t bytes = read(STDIN_FILENO, buf, sizeof(buf));

        if (bytes == -1) {
            const char msg[] = "error: failed to read from standard input\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            _exit(EXIT_FAILURE);
        }

        if (bytes > 0) {
            if (buf[bytes - 1] == '\n') {
                bytes--;
            }
            
            if (bytes == 0) {
                running = false;
                continue;
            }

            int r = rand() % 100;
            if (r < 80) {
                sem_wait(sem1);
                uint32_t *length1 = (uint32_t *)shm_buf1;
                char *text1 = shm_buf1 + sizeof(uint32_t);
                *length1 = bytes;
                memcpy(text1, buf, bytes);
                sem_post(sem1);
            } else {
                sem_wait(sem2);
                uint32_t *length2 = (uint32_t *)shm_buf2;
                char *text2 = shm_buf2 + sizeof(uint32_t);
                *length2 = bytes;
                memcpy(text2, buf, bytes);
                sem_post(sem2);
            }
        } else {
            running = false;
        }
    }

    sem_wait(sem1);
    uint32_t *length1 = (uint32_t *)shm_buf1;
    *length1 = UINT32_MAX;
    sem_post(sem1);

    sem_wait(sem2);
    uint32_t *length2 = (uint32_t *)shm_buf2;
    *length2 = UINT32_MAX;
    sem_post(sem2);

    waitpid(server1, NULL, 0);
    waitpid(server2, NULL, 0);

    sem_unlink(sem_name1);
    sem_close(sem1);
    sem_unlink(sem_name2);
    sem_close(sem2);
    munmap(shm_buf1, SHM_SIZE);
    munmap(shm_buf2, SHM_SIZE);
    shm_unlink(shm_name1);
    shm_unlink(shm_name2);
    close(shm1);
    close(shm2);
}