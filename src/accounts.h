#ifndef ACCOUNTS_H
#define ACCOUNTS_H

//constants for user log in/sign up
#define MAX_USERNAME_LEN 100
#define MAX_PASSWORD_LEN 100
#define MAX_LINE_LEN 256
#define ACCOUNTS_FILE "../data/accounts.txt"

int usernameExists(const char *username);
int checkPassword(const char *username, const char *password);
char* registerUser();
char* loginUser();

#endif
