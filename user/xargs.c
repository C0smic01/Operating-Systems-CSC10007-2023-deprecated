#include "../kernel/types.h"
#include "../kernel/param.h"
#include "user.h"

int main(int argc, char *argv[]) {
    char *xargv[MAXARG];
    char buffer[512];
    int buf_pos = 0, arg_pos = 0;

    if (argc < 2) {
        fprintf(2, "xargs: too few arguments\n");
        exit(1);
    }

    if (argc > MAXARG) {
        fprintf(2, "xargs: too much arguments\n");
        exit(1);
    }

    int count = 0;
    for (int i = 1; i < argc; i++) {
        xargv[count++] = argv[i];
    }

    while (read(0, &buffer[buf_pos], 1) > 0) {
        if (buf_pos > sizeof(buffer) - 1) {
            fprintf(2, "xargs: input too long");
        }

        if (buffer[buf_pos] == ' ' || buffer[buf_pos] == '\n') {
            buffer[buf_pos] = '\0';
            if (count < MAXARG - 1) {
                xargv[count++] = &buffer[arg_pos];
                arg_pos = buf_pos + 1;
            } else {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }
            
        } 
        
        if (buffer[buf_pos] == '\n' || buffer[buf_pos] == '\0') {
            xargv[count] = 0;
            if (fork() == 0) {
                exec(xargv[0], xargv);
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            } else {
                wait(0);
            }
            count = argc - 1;
            buf_pos = 0;
            arg_pos = 0;
            continue;
        }
        buf_pos++;
    }
    exit(0);
}

