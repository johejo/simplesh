#include "defs.h"

int main() {
    pid_t pid;
    int i, c;
    char tmp[LEN];
    int status;
    int pipe_c2p[2], pipe_p2c[2];
    char dir[256] = ".";
    char program_dir[PATH_MAX];
    
    getcwd(program_dir, PATH_MAX); //save program dir

    while (1) {
        i = 0;
        printf("PROMPT>");
        //get command
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

        // 'cd' is special
        if (!strncmp(cmd, "cd", strlen("cd"))) {
            if (opt == NULL) { // 'cd' only
                chdir(program_dir);
                strncpy(dir, program_dir, strlen(program_dir));
            }
            else { // 'cd' [PATH]
                chdir(opt);
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
            close(pipe_p2c[WRITE]);
            close(pipe_c2p[READ]);

            dup2(pipe_p2c[READ], 0);
            dup2(pipe_c2p[WRITE], 1);

            close(pipe_p2c[READ]);
            close(pipe_c2p[WRITE]);

            int check = 0;
            //cd is not binary
            if (!strncmp(cmd, "cd", strlen("cd"))) {
                check = execlp("pwd", "pwd", NULL); // confirmation
            } else {
                check = execlp(cmd, cmd, opt, NULL); // exec command
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
            pid_t r = waitpid(pid, &status, 0); //wait child
            if (r < 0) {
                perror("wait pid");
                exit(-1);
            }
            if (WIFEXITED(status)) {
                //child exit success
                char buf[LEN];
                memset(buf, '\0', LEN);
                read(pipe_c2p[READ], buf, LEN);
                write(1, buf, strlen(buf));
            } else {
                fprintf(stderr, "child status=%04x\n", status);
            }
        } else {
            //error
        }
    }

    return 0;
}
