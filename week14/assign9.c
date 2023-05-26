#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    unsigned i;
    int status;
    pid_t child_pid;

    printf("I am the Parent, my PID: %d\n", getpid());

    for (i = 0; i < 6; i++) {
        child_pid = fork();

        if (child_pid == -1) {
            perror("fork() failed");
            return 1;
        } else if (child_pid == 0) {
            printf("Child: PID: %d; PPID: %d\n", getpid(), getppid() );
            sleep(3);
            printf("Child %d says bye!\n", getpid());
            return 0;
        }
    }

    for (i = 0; i < 6; i++) {
        child_pid = wait(&status);
        if (child_pid == -1) {
            perror("wait() failed");
            return 1;
        }

        if (WIFEXITED(status)) {
            printf("Child %d exited with code %d\n", child_pid, WEXITSTATUS(status));
        } else {
            // Handle other cases here like WIFSIGNALED, WIFSTOPPED, etc...
            // See `man 2 wait` for more information.
        }
    }

    return 0;
}

