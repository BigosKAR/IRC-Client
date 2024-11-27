#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <conio.h>


#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define BUFFER_LEN 512

#define SERVER "irc.undernet.org"  
#define PORT 6667                   
#define CHANNEL "#testchannel"     
#define NICKNAME "TestBot"         
#define USERNAME "TestBot"   
#define CLIENT_EMAIL "irc295352@gmail.com" 
#define CLIENT_PASSWORD "testingircclient"    

SOCKET sockfd;

// SETUP FOR ALREADY REGISTERED USERS!
void setup(SOCKET sockfd, struct sockaddr_in *local_address){
    char message[BUFFER_LEN];
    int local_port = ntohs(local_address->sin_port);
    printf("Connected to %s on port %d\n", SERVER, PORT);

    snprintf(message, sizeof(message), "NICK %s\r\n", NICKNAME);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "USER %s 0 * :%s\r\n", USERNAME, USERNAME);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "JOIN %s\r\n", CHANNEL);
    send(sockfd, message, strlen(message), 0);

    snprintf(message, sizeof(message), "113 :%s :%d\r\n", NICKNAME, local_port);
    send(sockfd, message, strlen(message), 0);
}

void send_message(SOCKET sockfd, const char *message) {
    send(sockfd, message, strlen(message), 0);
    printf("Sent: %s", message);
}

void receive_messages(SOCKET sockfd) {
    char potential_message[512];
    char buffer[512];
    int bytesReceived;
    char *check = "PING ";

    bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived < 0) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK) {
            // No data to read, just return
            return;
        } else {
            printf("recv failed with error: %d\n", error);
            exit(1);
        }
    } else if (bytesReceived == 0) {
        printf("Connection closed by peer\n");
        exit(1);
    }
    buffer[bytesReceived] = '\0';
    printf("Received: %s", buffer);

    // Respond to PING command
    if (strstr(buffer, check) != NULL) {
        char *ping_message = strstr(buffer, check);
        if (ping_message != NULL) {
            char *ping_id = strchr(ping_message, ':');
            if (*ping_id != '\0') {
                snprintf(potential_message, sizeof(potential_message), "PONG %s\r\n", ping_id);
                //send_message(sockfd, potential_message); HIDE PONG MESSAGE FROM USER
                send(sockfd, potential_message, strlen(potential_message), 0);
            } else {
                printf("Error fetching the PING ID.\n");
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr, local_addr;
    struct hostent *host;
    char message[512];
    int local_addr_len = sizeof(local_addr);
    int local_port;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    host = gethostbyname(SERVER);
    if (host == NULL) {
        printf("Error resolving hostname: %s\n", SERVER);
        WSACleanup();
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // u_long mode;
    // ioctlsocket(sockfd, FIONBIO, &mode);  // Uncomment if you need to set the socket to unblocking mode
    // printf("Socket mode: %lu\n", mode);

    if (getsockname(sockfd, (struct sockaddr *)&local_addr, &local_addr_len) == SOCKET_ERROR) {
        printf("getsockname failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    setup(sockfd, &local_addr);

    fd_set read_fds, write_fds;
    struct timeval timeout;
    char input_buffer[BUFFER_LEN];


    // Main Receive/Send Functionality
    while (1) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        FD_SET(sockfd, &read_fds);
        FD_SET(sockfd, &write_fds); 

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &read_fds, &write_fds, NULL, &timeout);
        if (activity == SOCKET_ERROR) {
            printf("select() failed with error code: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            receive_messages(sockfd);
        }

        if (FD_ISSET(sockfd, &write_fds)) {
            if (kbhit()) // Works perfectly for waiting for user input!
            { 
                fgets(input_buffer, BUFFER_LEN, stdin);
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
