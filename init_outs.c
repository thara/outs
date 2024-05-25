#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// for macOS
#include <util.h>

// for debug
#define LOG_FILE "log.txt"

int main(int argc, char **argv) {
    const char* shell = getenv("SHELL");
    if (!shell) {
        shell = "/bin/sh";
    }

    int master_fd, slave_fd;
    char slave_name[100];

    if (openpty(&master_fd, &slave_fd, slave_name, NULL, NULL) == -1) {
        perror("openpty");
        exit(EXIT_FAILURE);
    }

    FILE *log_fp = fopen(LOG_FILE, "w");
    if (log_fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(log_fp, "[\n");

    pid_t pid;
    if ((pid == fork()) == 1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child
        close(master_fd);
        login_tty(slave_fd);

        const char* shell_name = strrchr(shell, '/');
        shell_name = shell_name ? shell_name + 1 : shell;
        execl(shell, shell_name, (char *)NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    } else { // parent
        close(slave_fd);

        char buf[256];
        ssize_t nread;
        int first = 1;

        while(0 < (nread = read(master_fd, buf, sizeof(buf)))) {
            buf[nread] = '\0';
            if (first) {
                first = 0;
            } else {
                fputs("\n", log_fp);
            }
            fputs(buf, log_fp);
        }

        waitpid(pid, NULL, 0); // wait to finish the child process
        fclose(log_fp);
    }

    return 0;
}
