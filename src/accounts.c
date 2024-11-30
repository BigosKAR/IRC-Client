#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "accounts.h"

//FUNCTIONS FOR USER LOG IN/SIGN UP

int usernameExists(const char *username) {
    FILE *fp = fopen(ACCOUNTS_FILE, "r");
    if (!fp) {
        //if file doesn't exist, no accounts exist yet.
        return 0;
    }
    char line[MAX_LINE_LEN];
    char storedUsername[MAX_USERNAME_LEN];
    char storedPassword[MAX_PASSWORD_LEN];
    while (fgets(line, sizeof(line), fp)) {
        //each line is: username password
        if (sscanf(line, "%s %s", storedUsername, storedPassword) == 2) {
            if (strcmp(username, storedUsername) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int checkPassword(const char *username, const char *password) {
    FILE *fp = fopen(ACCOUNTS_FILE, "r");
    if (!fp) {
        //if file doesn't exist, no accounts exist yet.
        return 0;
    }
    char line[MAX_LINE_LEN];
    char storedUsername[MAX_USERNAME_LEN];
    char storedPassword[MAX_PASSWORD_LEN];
    while (fgets(line, sizeof(line), fp)) {
        //each line is: username password
        if (sscanf(line, "%s %s", storedUsername, storedPassword) == 2) {
            if (strcmp(username, storedUsername) == 0) {
                fclose(fp);
                if (strcmp(password, storedPassword) == 0) {
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

char* registerUser() {
    char *username = (char*)malloc(MAX_USERNAME_LEN * sizeof(char));
    if (!username) {
        printf("\n|ERROR| - Memory allocation failure.\n");
        return NULL;
    }

    char password[MAX_PASSWORD_LEN];
    printf("\n\nSIGN UP\n");
    printf("---- --");

    while (1) {
        printf("\nEnter a username: ");
        if (scanf("%s", username) != 1) {
            printf("\nInvalid input. Please try again.\n");
            continue;
        }
        while (getchar() != '\n');

        if (usernameExists(username)) {
            printf("\nUsername '%s' is already taken. Please try another one.\n", username);
        } else {
            break;
        }
    }

    printf("\nEnter a password: ");
    if (scanf("%s", password) != 1) {
        printf("\nInvalid input.\n");
        free(username); 
        return NULL;
    }
    while (getchar() != '\n');

    FILE *fp = fopen(ACCOUNTS_FILE, "a");
    if (!fp) {
        printf("\nError opening accounts file.\n");
        free(username); 
        return NULL;
    }
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);

    printf("\nAccount registered successfully!\n\n\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n\n\n\n\n\n");
    sleep(1);
    return username;
}

char* loginUser() {
    char *username = (char*)malloc(MAX_USERNAME_LEN * sizeof(char));
    if (!username) {
        printf("\n|ERROR| - Memory allocation failure.\n");
        return NULL;
    }

    char password[MAX_PASSWORD_LEN];
    printf("\n\nLOGIN\n");
    printf("----");

    while (1) {
        printf("\nEnter your username: ");
        if (scanf("%s", username) != 1) {
            printf("\nInvalid input. Please try again.\n");
            continue;
        }
        while (getchar() != '\n');

        if (!usernameExists(username)) {
            printf("\nUsername '%s' does not exist. Please try again.\n", username);
        } else {
            break;
        }
    }

    while (1) {
        printf("\nEnter your password: ");
        if (scanf("%s", password) != 1) {
            printf("\nInvalid input. Please try again.\n");
            continue;
        }
        while (getchar() != '\n');

        if (checkPassword(username, password)) {
            printf("\nLogin successful!\n\n\n");
            printf("------------------------------------------------------------------------------------------------------------------------\n\n\n\n\n\n");
            sleep(1);
            break;
        } else {
            printf("\nIncorrect password. Please try again.\n");
        }
    }

    return username;
}