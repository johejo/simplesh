#include "defs.h"

int main() {
    pid_t pid;
    int i, c;
    char tmp[LEN];
    int status;
    int pipe_c2p[2], pipe_p2c[2];

    while (1) {
        i = 0;
        printf("PROMPT>");
        while ((c = getchar()) != '\n') {
            tmp[i++] = (char) c;
            if (i >= LEN - 1) {
                break;
            }
        }
        tmp[i] = '\0';
        char *arg = tmp;
        char cmd[] = "";
        strcpy(cmd, tmp);
        char *opt = strstr(cmd, " ");
        if (opt != NULL) {
            opt++;
            printf("opt %s\n", opt);
        }

        int n = (int) strcspn(tmp, " ");
        cmd[n] = '\0';


        printf("cmd %s\n", cmd);


        printf("arg %s\n\n", arg);

        if (!strncmp("exit", tmp, LEN)) {
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
//            printf("child %d\n", pid);
//            printf("%s\n", option);

            close(pipe_p2c[WRITE]);
            close(pipe_c2p[READ]);

            dup2(pipe_p2c[READ], 0);
            dup2(pipe_c2p[WRITE], 1);

            close(pipe_p2c[READ]);
            close(pipe_c2p[WRITE]);

//            printf("%s\n", arg);
            int check = execlp(cmd, cmd, opt, NULL);
            if (check < 0) {
                perror("error");
                close(pipe_p2c[WRITE]);
                close(pipe_c2p[READ]);
                exit(1);
            }
        } else if (pid > 0) {
            //parent
//            printf("parent %d\n", pid);
            close(pipe_p2c[READ]);
            close(pipe_c2p[WRITE]);
            pid_t r = waitpid(pid, &status, 0); //子プロセスの終了待ち
            if (r < 0) {
                perror("waitpid");
                exit(-1);
            }
            if (WIFEXITED(status)) {
                // 子プロセスが正常終了の場合
                char buf[LEN];
                memset(buf, '\0', LEN);
                read(pipe_c2p[READ], buf, LEN);
                write(1, buf, strlen(buf));
//                printf("child exit-code=%d\n", WEXITSTATUS(status));
            } else {
                printf("child status=%04x\n", status);
            }
        } else {
            //error
        }
    }

    return 0;
}
