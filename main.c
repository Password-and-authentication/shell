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
        }
        int rc = fork();
        if (rc == 0) {
            arg = strsep(&line, " \n\t\0");
            if (strcmp(arg, "path") == 0) {
                addpaths(line, paths);
                printf("%s\n", paths[1]);
            } else {
                char *path;
                if ((path = checkaccess(arg, paths))) {
                    argv[0] = path;
                }
                getargs(argv, line);
                execv(argv[0], argv);     
            }
    
        } else wait(NULL);
        free(lineptr);
        line = NULL;
        len = 0;
    }
}

void addpaths(char *line, char *paths[]) {
    char *path;

    while (*line) {
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



