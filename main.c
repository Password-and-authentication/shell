#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>


char *setpath(char *line);
bool getargs(char *args[], char **line);
int whitespace(char **line);
char *checkaccess(char *arg, char *paths[]);
void freearr(char *args[], size_t len);
void addpaths(char *line, char *paths[]);
void changedir(char *line);

const char error_message[30] = "An error has occurred\n";
void printerr();


int main(int argc, char *argv[]) {


    char *line = NULL;
    char *lineptr;
    size_t len = 0;
    char *arg;
    char *args[10];
    char *paths[10];
    FILE* file = stdin;
    paths[0] = strdup("/usr/bin/");
    paths[1] = NULL;

    if (argc == 2) {
        file = fopen(argv[1], "r");
    }

    while (1) {
        if (argc == 1) printf("wish> ");

        if (getline(&line, &len, file) == EOF)
            break;
        lineptr = line;
        whitespace(&line);
        
        if (strncmp("exit", line, 4) == 0) {
            exit(0);
        } else if (strncmp("cd", line, 2) == 0) {
            line += 2;
            int end = whitespace(&line);
            if (end)
                printerr();
            else {
                arg = strsep(&line, " \n\t");
                if (*line != 0) 
                    printerr();
                else changedir(arg);
            }
                
        } else if (strncmp("path", line, 4) == 0) {
            line += 4;
            int end = whitespace(&line);
            if (end)
                *paths = NULL;
            else addpaths(line, paths);
            
        } else {
            bool parallel;
            int n = 0;
            while (1) {
                n++;
                parallel = getargs(args, &line);

                int rc = fork();
                if (rc == 0) {
                    if (*paths == NULL)
                        printerr();
                    else
                        args[0] = checkaccess(args[0], paths);
                        if (args[0] == NULL)
                            printerr();
                        else
                            execv(args[0], args);
                }
                
                for (int i = 0; i < 10; ++i)
                    args[i] = NULL;
                if (!parallel) break;
            }
            while (n--) {
                wait(NULL);
            }
        }
    }
}

void changedir(char *arg) {
    if ((chdir(arg)) < 0) printerr();
}

void addpaths(char *line, char *paths[]) {
    char *path;
    while (*line) {
        whitespace(&line);
        path = strsep(&line, " \n\t");
        *paths++ = strdup(path);     
    }
    *paths = NULL;
}

char *checkaccess(char *arg, char *paths[]) {
    char *path;
    while (*paths && (path = strdup(*paths++))) {
        strcat(path, arg);
        if (access(path, X_OK) == 0) {
            return path;
        }
        free(path);
    }
    return NULL;
}

bool getargs(char *args[], char **line) {
    char *arg;
    bool rdirect;
    int i = 1;
    int end = whitespace(line);
    if (!end) {
        while (**line) {
            arg = strsep(line, " \n\t\0");
            //printf("%s\n", arg);
            if (strcmp(arg, "&") == 0) {
                return 1;
            }
            if (strncmp(arg, ">", 1) == 0) {  
                if (rdirect) 
                    printerr();
                else {
                    if (*line != 0) {

                    close(STDOUT_FILENO);
                    whitespace(line);
                    arg = strsep(line, " \n\t");
                    open(arg, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
                    }
                }
                rdirect = 1;
            } else {
                //printf("%s\n", arg);
                *args++ = strdup(arg);
                if (*line == NULL) break;
                if (whitespace(line))
                    break;
            }
        }
    }
    *++args = NULL;
    return 0;
}  


int whitespace(char **line) {
    while (1) {
        if (**line == 0 || **line == EOF) return 1;
        if (**line != ' ' && **line != '\n' && **line != '\t')
                break;
        *line += 1;
    }
    return 0;
}

void freearr(char *args[], size_t len) {
    
    while (len--) {
        printf("e");
    }
}

void printerr() {
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}