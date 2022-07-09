#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char *setpath(char *line);
void getargs(char *argv[], char *line);
int whitespace(char **line);
char *checkaccess(char *arg, char *paths[]);
void freearr(char *argv[], size_t len);
void addpaths(char *line, char *paths[]);

const char error_message[30] = "An error has occurred\n";
void printerr();


int main() {

    char *line = NULL;
    char *lineptr;
    size_t len = 0;
    char *arg;
    char *argv[10];
    char *paths[10];
    paths[0] = strdup("/usr/bin/");
    paths[1] = strdup("/bin/");
    paths[2] = NULL;

    while (1) {
        getline(&line, &len, stdin);
        lineptr = line;
        whitespace(&line);
        
        if (strncmp("exit", line, 4) == 0) {
            exit(0);
        } else if (strncmp("cd", line, 2) == 0) {
            line += 2;
            int end = whitespace(&line);
            if (end)
                printerr();
        } else if (strncmp("path", line, 4) == 0) {
            line += 4;
            int end = whitespace(&line);
            if (end)
                *paths = NULL;
            else addpaths(line, paths);
            
        } else {
            int rc = fork();
            if (rc == 0) {
                if (*paths == NULL) {
                    printf("No paths set\n");
                    exit(0);
                } else {
                    arg = strsep(&line, " \n\t\0");
                    char *path;
                    if ((path = checkaccess(arg, paths))) {
                        argv[0] = path;
                    }
                    getargs(argv, line);
                    execv(argv[0], argv);
                }

            } else wait(NULL);
        }
    }
}

void addpaths(char *line, char *paths[]) {
    char *path;
    while (*line) {
        whitespace(&line);
        path = strsep(&line, " \n\t");
        *paths++ = strdup(path);     
    }
    while ((path = *paths++))
        strcpy(path, " ");
}

char *checkaccess(char *arg, char *paths[]) {
    char *path;
    while ((path = strdup(*paths++))) {
        strcat(path, arg);
        //printf("%s\n", path);
        if (access(path, X_OK) == 0) {
            return path;
        }
        free(path);
    }
    return NULL;
}

void getargs(char *argv[], char *line) {
    char *arg;
    int i = 1;
    int end = whitespace(&line);
    if (!end) {
        while (*line) {
            arg = strsep(&line, "> \n\t\0");
            argv[i++] = strdup(arg);
            whitespace(&line);
        }
    }

    argv[i] = NULL;
}   

int whitespace(char **line) {
    while (1) {
        if (**line == 0) return 1;
        if (**line != ' ' && **line != '\n' && **line != '\t')
                break;
        *line += 1;
    }
    return 0;
}

void freearr(char *argv[], size_t len) {
    
    while (len--) {
        printf("e");
    }
}

void printerr() {
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}



