#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include <unistd.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib


//constants for user log in/sign up

#define MAX_USERNAME_LEN 100
#define MAX_PASSWORD_LEN 100
#define MAX_LINE_LEN 256
#define ACCOUNTS_FILE "accounts.txt"


//constants for irc_client
#define BUFFER_LEN 512

#define SERVER "Oslo.NO.EU.UnderNet.org"  
#define PORT 6667                   
#define CHANNEL "#testchannel"     
#define NICKNAME "lsdsañkdmcpsjksakcajncsdk"          
#define CLIENT_EMAIL "default@gmail.com" 
#define CLIENT_PASSWORD "testingircclient"    

SOCKET sockfd;


//function prototypes

    //irc_client
int run_client(char *username);
char *format_command(const char *input);
void setup(char *username, SOCKET sockfd, struct sockaddr_in *local_address);
void send_message(SOCKET sockfd, const char *message);
void format_string(char *string);
void receive_messages(SOCKET sockfd);

    //user loggin
int usernameExists(const char *username);
int checkPassword(const char *username, const char *password);
char *registerUser();
char *loginUser();

    //commands and others
void sub_menu();
void show_file_info(char *filename);

    //menu function
char *menu();



int main() {
    int result;
    char username[MAX_PASSWORD_LEN];

    strcpy(username, menu());
    if (strcmp(" ", username) != 0){
        result = run_client(username);

        if(result == 1){
            return result;
        }
    }

    return result;
}


//FUNCTIONS FOR IRC_CLIENT

int run_client(char *username) {

    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr, local_addr;
    struct hostent *host;
    char message[512];
    int local_addr_len = sizeof(local_addr);
    int local_port;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("|ERROR| - WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    host = gethostbyname(SERVER);
    if (host == NULL) {
        printf("|ERROR| - Error resolving hostname: %s\n", SERVER);
        WSACleanup();
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("|ERROR| - Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("|ERROR| - Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    if (getsockname(sockfd, (struct sockaddr *)&local_addr, &local_addr_len) == SOCKET_ERROR) {
        printf("|ERROR| - getsockname failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    setup(username, sockfd, &local_addr);

    fd_set read_fds, write_fds;
    struct timeval timeout;
    char input_buffer[BUFFER_LEN];

    //main Receive/Send Functionality
    while (true) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        FD_SET(sockfd, &read_fds);
        FD_SET(sockfd, &write_fds); 

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &read_fds, &write_fds, NULL, &timeout);
        if (activity == SOCKET_ERROR) {
            printf("|ERROR| - select() failed with error code: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            receive_messages(sockfd);
        }

        if (FD_ISSET(sockfd, &write_fds)) {
            if (kbhit()) //waiting for user input
            { 
                printf("\n=====================> %s: ", username);
                fgets(input_buffer, BUFFER_LEN, stdin);
                printf("\n");
                input_buffer[strcspn(input_buffer, "\n")] = '\0'; 

                if (strlen(input_buffer) > 0) {
                    char send_msg[BUFFER_LEN + 2];
                    snprintf(send_msg, sizeof(send_msg), "%s\r\n", input_buffer);
                    send_message(sockfd, send_msg);
                }
            }
        }
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
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

//setup for already registered users
void setup(char *username, SOCKET sockfd, struct sockaddr_in *local_address){
    char message[BUFFER_LEN];
    int local_port = ntohs(local_address->sin_port);
    printf("Connected to %s on port %d\n", SERVER, PORT);

    snprintf(message, sizeof(message), "NICK %s\r\n", NICKNAME);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "USER %s 0 * :%s\r\n", username, username);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "JOIN %s\r\n", CHANNEL);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "113 :%s :%d\r\n", NICKNAME, local_port);
    send(sockfd, message, strlen(message), 0);
    
    snprintf(message, sizeof(message), "REGISTER %s %s\r\n", CLIENT_EMAIL, CLIENT_PASSWORD);
    send(sockfd, message, strlen(message), 0);
}

void send_message(SOCKET sockfd, const char *message) {
    char *formatted = format_command(message);
    if (formatted == NULL) {
        return;
    }
    send(sockfd, formatted, strlen(formatted), 0);
    printf("Sent: %s\r\n", formatted);
    printf("\n");
}

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

//modify receive_messages to handle relevant IRC messages and filter out the unwanted ones
void receive_messages(SOCKET sockfd) {
    char potential_message[BUFFER_LEN];
    char buffer[BUFFER_LEN], aux_buffer[BUFFER_LEN];
    int bytesReceived;
    char *check = "PING ";

    bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived < 0) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) {
            //no data to read, just return
            return;
        } else {
            printf("|ERROR| - recv failed with error: %d\n", WSAGetLastError());
            exit(1);
        }
    } else if (bytesReceived == 0) {
        printf("Connection closed by peer\n\n\n");
        printf("Thanks for using our irc_client! Hope to see you again.\n\n\n");
        exit(1);
    }
    buffer[bytesReceived] = '\0';

    strcpy(aux_buffer, buffer);
    format_string(aux_buffer);  //format the string (removes unnecessary parts)

    //only print messages that are not empty or irrelevant
    if (strlen(aux_buffer) > 0) {
        printf("Received: %s\n", aux_buffer);  //print the cleaned up message
        printf("\n"); 
    }

    //respond to PING command (ignore if not needed)
    if (strstr(buffer, check) != NULL) {
        char *ping_message = strstr(buffer, check);
        if (ping_message != NULL) {
            char *ping_id = strchr(ping_message, ':');
            if (*ping_id != '\0') {
                snprintf(potential_message, sizeof(potential_message), "PONG %s\r\n", ping_id);
                send(sockfd, potential_message, strlen(potential_message), 0);  //hide PONG from user
            } else {
                printf("|ERROR| - Couldn't fetch the PING ID.\n");
            }
        }
    }
}




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


//COMMANDS AND OTHERS FUNCTIONS
void sub_menu(){
    int choice;

    
    do{
        choice = -1;

        printf("\n\nOTHER UTILITIES\n");
        printf("----- ---------\n\n");
        printf("1. Useful commands\n");
        printf("2. Show other servers' information\n");
        printf("3. Users security and recomendations\n");
        printf("0. Return to main menu\n\n");
        printf("Please enter your choice: ");

        fflush(stdin);
        getchar();
        scanf("%i", &choice);
        printf("\n\n");

        switch (choice)
        {
        case 0:
            printf("Exiting the submenu...\n\n");
            sleep(1);
            break;
        case 1:
            printf("\n\n");
            show_file_info("commands.txt");
            printf("\n\n");
            system("pause");
            printf("\n\n");
            break;
        
        case 2:
            printf("\n\n");
            show_file_info("servers.txt");
            printf("\n\n");
            system("pause");
            printf("\n\n");
            break;
        
        case 3:
            printf("\n\n");
            show_file_info("recommendations.txt");
            printf("\n\n");
            system("pause");
            printf("\n\n");
            break;

        default:
            printf("|ERROR| - Invalid input\n");
            printf("\n\n");
            system("pause");
            printf("\n\n");
            break;
        }


    }while(choice != 0);

    return;
}

void show_file_info(char *filename) {
    FILE *fich;
    char current_line[BUFFER_LEN + 1];

    fich = fopen(filename, "r");
    if (fich == NULL) {
        printf("|EROR| - Could not open file '%s'.\n", filename);
        return;
    }
 
    fgets(current_line, BUFFER_LEN, fich);

    while(!feof(fich)){
        printf("%s", current_line);
        fgets(current_line, BUFFER_LEN, fich);
    }

    fclose(fich);
}



//MENU FUNCTION
char* menu() {
    int choice = -1;
    char *username = (char*)malloc(MAX_USERNAME_LEN * sizeof(char));
    if (!username) {
        printf("\nMemory allocation failed.\n");
        return NULL;
    }

    printf("\n\n");
    printf("\t\t\t\t\t\tIRC CLIENT\n");
    printf("\t\t\t\t\t\t----------\n");

    
    do{
        printf("MENU\n");
        printf("----\n\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Commands and others\n");
        printf("0. Exit\n\n");
        printf("Please enter your choice: ");

        fflush(stdin);
        scanf("%i", &choice);

        switch (choice) {
            case 0:
                printf("Exiting the program...\n");
                sleep(2);
                printf("\nGoodbye!\n");
                free(username); 
                exit(0);
            case 1:
                strcpy(username, registerUser());
                return username;
            case 2:
                strcpy(username, loginUser());
                return username;
            case 3:
                sub_menu();
                printf("\n\nPress enter to continue: ");
                fflush(stdin);
                getchar();
                break;
            default:
                printf("\n|ERROR| - Invalid input, try again\n\n");
                system("pause");
                printf("\n\n");
        }
    }while(choice != 0);

    free(username); 
    return NULL;
}