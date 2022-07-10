#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>


char *setpath(char *line);
bool getargs(char *argv[], char **line);
int whitespace(char **line);
char *checkaccess(char *arg, char *paths[]);
void freearr(char *argv[], size_t len);
void addpaths(char *line, char *paths[]);
void changedir(char *line);

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
                parallel = getargs(argv, &line);

                int rc = fork();
                if (rc == 0) {
                    if (*paths == NULL)
                        printerr();
                    else
                        argv[0] = checkaccess(argv[0], paths);
                        if (argv[0] == NULL)
                            printerr();
                        else
                            execv(argv[0], argv);
                }
                
                for (int i = 0; i < 10; ++i)
                    argv[i] = NULL;
                if (!parallel) break;
            }
            while (n--) {
                wait(NULL);
            }
            printf("both ended\n");
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
    *++paths = NULL;
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

bool getargs(char *argv[], char **line) {
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
                *argv++ = strdup(arg);
                whitespace(line);
            }
        }
    }
    *++argv = NULL;
    return 0;
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