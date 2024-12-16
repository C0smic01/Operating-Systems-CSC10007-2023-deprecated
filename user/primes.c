#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

void primes(int readp) __attribute__((noreturn));

void primes(int readp) {
    int prime, n;
    int p[2];

    if (pipe(p) < 0) {
        fprintf(2, "primes: can not create pipe\n");
        exit(1);
    }

    if (read(readp, &prime, sizeof(int)) != sizeof(int)) {
        exit(1);
    }

    printf("prime %d\n", prime);

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        close(p[1]);
        close(readp);
        primes(p[0]);
    } else {
        close(p[0]);
        while (read(readp, &n, sizeof(int)) == sizeof(int)) {
            if (n % prime != 0) {
                write(p[1], &n, sizeof(int));
            }
        }
        close(p[1]);
        close(readp);
        wait(0);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int p[2];

    if (pipe(p) < 0) {
        fprintf(2, "primes: can not create pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        exit(1);
    }
    
    if (pid == 0) {
        close(p[1]);
        primes(p[0]);
    } else {
        close(p[0]);
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}
