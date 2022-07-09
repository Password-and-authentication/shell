#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char *setpath(char *line);
void getargs(char *argv[], char *line);
char *whitespace(char *line);


int main() {

    char *line = NULL;
    size_t len = 0;
    char *arg;
    char *path = strdup("/usr/bin/");
    char d = ' ';

    while (1) {
        getline(&line, &len, stdin);

        line = whitespace(line);
        
        if (strncmp("exit", line, 4) == 0) {
            exit(0);
        }
        int rc = fork();
        if (rc == 0) {
            arg = strsep(&line, " \n\t\0");
            if (strcmp(arg, "path") == 0) {
                path = line;

            } else {
                char *argv[10];
                argv[0] = strdup(path);
                argv[0] = strcat(argv[0],arg);
                getargs(argv, line);

                execv(argv[0], argv);
            }
        } else wait(NULL); 
    }
}

void getargs(char *argv[], char *line) {
    char *arg;
    int i = 1;


    while (*line != 0 && (arg = strsep(&line, " \t\n\0"))) {
        argv[i++] = arg;
    }


    argv[i] = NULL; 
    return;
}   

char *whitespace(char *line) {
    while (1) {
        if (*line != 0 && *line != 32 && *line != '\n' && *line != '\t')
            break;
        ++line;
    }
    return line;
}

