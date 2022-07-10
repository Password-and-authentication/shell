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
    size_t len = 0;
    char *arg;
    char *args[10];
    char *paths[10];
    FILE* file = stdin;
    paths[0] = strdup("/usr/bin/");     // default path
    paths[1] = NULL;

    if (argc == 2) {
        file = fopen(argv[1], "r");
    }

    while (1) {
        if (argc == 1) printf("wish> ");

        if (getline(&line, &len, file) == EOF)
            break;
        whitespace(&line);
         
        if (strncmp("exit", line, 4) == 0) {
            exit(0);
        } else if (strncmp("cd", line, 2) == 0) {

            // skip "cd"
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

            // skip "path"
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
                // parallel is true if there is &
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
                    args[i] = NULL;         // reset
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


// makes a new paths list
void addpaths(char *line, char *paths[]) {
    char *path;
    while (*line) {
        whitespace(&line);
        path = strsep(&line, " \n\t");
        *paths++ = strdup(path);     
    }
    *paths = NULL;
}

// checks if the program exists in any of the paths, for example "ls" exists in /usr/bin/
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

// returns true if there is a "&", 
// which means there are multiple commands
bool getargs(char *args[], char **line) {
    char *arg;
    bool rdirect;
    int end = whitespace(line);
    if (!end) {
        while (**line) {
            arg = strsep(line, " \n\t\0");
            //printf("%s\n", arg);
            if (strcmp(arg, "&") == 0) {
                return 1;
            }
            if (strncmp(arg, ">", 1) == 0) {  

                if (rdirect) // true if ">" was already encountered
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
                *args++ = strdup(arg);
                if (*line == NULL) break;
                whitespace(line);
            }
        }
    }
    *++args = NULL;
    return 0;
}  

// remove whitespace and return true if last argument
int whitespace(char **line) {
    while (1) {
        if (**line == 0 || **line == EOF) return 1;
        if (**line != ' ' && **line != '\n' && **line != '\t')
                break;
        *line += 1;
    }
    return 0;
}

void printerr() {
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}