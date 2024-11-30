#include "formatting.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFER_LEN 512

//function to clean up unwanted tokens (e.g., NOTICE messages) and format the string
void format_string(char *string) {
    char *token, aux_string[BUFFER_LEN];
    int count = 0;
    char *delimiter = ":;~";

    strcpy(aux_string, string);

    string[0] = '\0';

    token = strtok(aux_string, delimiter);

    while (token != NULL) {
        if (count == 0) {
            token = strtok(NULL, delimiter);
            count = 1;
            continue;
        }
        if (strstr(token, "NOTICE") == NULL) {
            strcat(string, token);
            strcat(string, " ");
        }
        token = strtok(NULL, delimiter);
    }
    string[strlen(string)] = '\0';
}

char *format_command(const char *input) {
    static char formatted[BUFFER_LEN];
    int count = 0;
    int len = strlen(input);
    bool valid_format = false;

    while (count < len && input[count] == ' ') {
        count++; 
    }

    if (input[count] == '/') {
        valid_format = true;
        count++;
    }

    if (valid_format) {
        strncpy(formatted, input + count, len - count);
        formatted[len - count] = '\0';
        return formatted;
    } else {
        printf("\n|ERROR| - Invalid format. Commands must begin with '/'.\n\n");
        return NULL;
    }
}