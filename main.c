#include "defs.h"

int main() {
    pid_t pid;
    int i, c;
    char tmp[LEN];
    int status;
    int pipe_c2p[2], pipe_p2c[2];
    char dir[256] = ".";
    char program_dir[PATH_MAX];
    
    getcwd(program_dir, PATH_MAX);
//    printf("%s\n", program_dir);

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
        char cmd[] = "";
        strcpy(cmd, tmp);
        char *opt = strstr(cmd, " ");
        if (opt != NULL) {
            opt++;
        }

        int n = (int) strcspn(tmp, " ");
        cmd[n] = '\0';

        if (!strncmp("exit", tmp, LEN)) {
            break;
        }

        if (!strncmp(cmd, "cd", strlen("cd"))) {
            if (opt == NULL) {
                int n = chdir(program_dir);
                strncpy(dir, program_dir, strlen(program_dir));
            }
            else {
                int n = chdir(opt);
                strncpy(dir, opt, strlen(opt));
            }
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

            int check = 0;
//            printf("%s\n", arg);
            if (!strncmp(cmd, "cd", strlen("cd"))) {
                check = execlp("pwd", "pwd", NULL);
            } else {
                int n = chdir(dir);
//                printf("dir %s\n\n", dir);
                check = execlp(cmd, cmd, opt, NULL);
            }
            if (check < 0) {
                perror("error");
                close(pipe_p2c[WRITE]);
                close(pipe_c2p[READ]);
                exit(1);
            }
        } else if (pid > 0) {
            //parent
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
            } else {
                printf("child status=%04x\n", status);
            }
        } else {
            //error
        }
    }

    return 0;
}
