#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2, pid3, pid4;
    int status;

    pid1 = -1;
    pid2 = -1;
    pid3 = -1;
    pid4 = -1;

    pid1 = fork();

    if (pid1 == 0) {
        pid2 = fork();
        if (pid2 == 0) {
            // bottom half of the first floor
            execl("./c", "/c", NULL);
        } else {
            // top half of the first floor
            execl("./b", "/b", NULL);
        }
    } else {
        // wait for the first floor to finish
        waitpid(pid2, &status, 0);

        pid3 = fork();

        if (pid3 == 0) {
            // bottom half of the second floor
            execl("./c", "/c", NULL);
        } else {
            // top half of the second floor
            execl("./b", "/b", NULL);
        }

        // wait for the second floor to finish
        waitpid(pid3, &status, 0);

        // roof of the house
        execl("./a", "/a", NULL);

        // wait for all child processes to finish
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
        waitpid(pid3, &status, 0);
        waitpid(pid4, &status, 0);
    }

    return 0;
}

