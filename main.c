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
        //printf("s");

        int rc = fork();
        if (rc == 0) {

            arg = strsep(&line, " \n");
            
            if (strcmp(arg, "path") == 0) {
                path = line;

            } else {
                char *argv[10];
                argv[0] = strdup(path);
                argv[0] = strcat(argv[0],arg);
                getargs(argv, line);

            
                //execv(argv[0], argv);
            }
        } else wait(NULL);      
    }
}

void getargs(char *argv[], char *line) {
    char *arg;
    line = whitespace(line);
    //printf("%d\n", *line);
    //printf("%d\n", *line);
    arg = strsep(&line, " \n");
    printf("%d\n", *arg);
}   

char *whitespace(char *line) {
    while (*line == ' ' || *line == '\n' || *line++ == '\t')
            ;
    return --line;
}

