#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void parent_handler(int signo) {
    printf("Parent: Received signal %d\n", signo);
}

void child1_handler(int signo) {
    printf("Child 1: Received signal %d\n", signo);
}

void child2_handler(int signo) {
    printf("Child 2: Received signal %d\n", signo);
}

int main() {
    pid_t pid1, pid2;

    // Set up signal handlers for parent process
    signal(SIGUSR1, parent_handler);
    signal(SIGUSR2, parent_handler);

    // Create child 1 process
    pid1 = fork();
    if (pid1 == 0) {
        // Set up signal handlers for child 1 process
        signal(SIGUSR1, child1_handler);

        // Child 1 process waits for signal from parent
        while(1) {
            pause();
        }
    }

    // Create child 2 process
    pid2 = fork();
    if (pid2 == 0) {
        // Set up signal handlers for child 2 process
        signal(SIGUSR2, child2_handler);

        // Child 2 process waits for signal from parent
        while(1) {
            pause();
        }
    }

    // Parent process sends signals to child processes
    while(1) {
        sleep(1);
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR2);
    }

    return 0;

}
