#include "defs.h"

int main() {
    pid_t pid;
    int i;
    int c;
    char cmd[CMD_LEN];
    int status;
    int pipe_c2p[2], pipe_p2c[2];

    while (1) {
        i = 0;
        printf("PROMPT>");
        while ((c = getchar()) != '\n') {
            cmd[i++] = (char) c;
            if (i >= CMD_LEN - 1) {
                break;
            }
        }

        if (!strncmp("exit", cmd, CMD_LEN)) {
            break;
        }

        if (pipe(pipe_c2p) < 0){
            //error
            perror("pipe_c2p error");
        }

        if (pipe(pipe_p2c) < 0){
            //error
            perror("pipe_p2c error");
        }

        pid = fork();
        if (pid == 0) {
            //child
            printf("%d\n", pid);


            close(pipe_p2c[WRITE]);
            close(pipe_c2p[READ]);

            dup2(pipe_p2c[READ], 0);
            dup2(pipe_c2p[WRITE], 1);

            close(pipe_p2c[READ]);
            close(pipe_c2p[WRITE]);
            if (execlp(cmd, NULL) < 0) {
                perror("error");
                close(pipe_p2c[WRITE]);
                close(pipe_c2p[READ]);
                exit(1);
            }
        } else if (pid > 0) {
            //parent
            printf("%d\n", pid);
            close(pipe_p2c[READ]);
            close(pipe_c2p[WRITE]);
            pid_t wait = waitpid(pid, &status, 0);
            if (wait < 0) {
                //error
            }
        } else {
            //error
        }
    }

    return 0;
}
