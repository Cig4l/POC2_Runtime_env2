#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <bits/signum-generic.h>

int main(void) {
    char* path_ok = "bots/bot_ok.py";
    char* path_loop = "./bots/bot_loop.py";
    char* path_slow = "./bots/bot_slow.py";

    char *args[] = {"python3", path_ok, NULL};

    int out[2]; // out[0] = lecture (parent), out[1] = écriture (bot)
    pipe(out);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // ENFANT : destiné à devenir le bot
        printf("%s\n", "Child");
        dup2(out[1], STDOUT_FILENO);
        execvp(args[0], args);
    } else {
        // PARENT : reste le programme principal, pid = numéro de l'enfant
        struct pollfd pfd = { .fd = out[0], .events = POLLIN };
        int ready = poll(&pfd, 1, 500); // attend au plus 500 ms

        if(ready > 0) {
            printf("%s\n", "OK PROCESS");
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            printf("%s\n", "Process terminé !");
        } else if(ready == 0) {
            printf("%s\n", "TIMEOUT");
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            printf("%s\n", "Process terminé !");
        } else if(ready < 0) {
            printf("%s\n", "ERROR");
            kill(pid, SIGKILL);
            waitpid(pid, NULL, 0);
            printf("%s\n", "Process terminé !");
        }
    }
    return 0;
}