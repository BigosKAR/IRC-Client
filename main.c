#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <string.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define BUFFER_LEN 512

#define SERVER "irc.undernet.org"  
#define PORT 6667                  
#define CHANNEL "#testchannel"     
#define NICKNAME "TestBot"         
#define USERNAME "TestBot"   
#define CLIENT_EMAIL "irc295352@gmail.com" 
#define CLIENT_PASSWORD "testingircclient"    

// HANDLE mutex;
SOCKET sockfd;

DWORD WINAPI ThreadFunction(LPVOID lpParam){
    char send_buffer[BUFFER_LEN];
    while(1){
        printf("Enter message: ");
        fgets(send_buffer, BUFFER_LEN, stdin);
        send_buffer[strcspn(send_buffer, "\n")] = '\0';
        // WaitForSingleObject(mutex, INFINITE);
        send(sockfd, send_buffer, strlen(send_buffer), 0);
        // ReleaseMutex(mutex); 
    }

    return 0;
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

    while (1) {
        // WaitForSingleObject(mutex, INFINITE);
        bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 1) {
            printf("Connection closed or error\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("Received: %s", buffer);
        // ANSWERING TO PING RESPONSE
        if (strstr(buffer, check)!=NULL){
            char *ping_message = strstr(buffer, check);
            if (ping_message != NULL) {
                char *ping_id = strchr(ping_message, ':');
                if (*ping_id != '\0') {
                    snprintf(potential_message, sizeof(potential_message), "PONG %s\r\n", ping_id);
                    send_message(sockfd, potential_message);
                } else {
                    printf("Error fetching the PING ID.\n");
                }
            } else {
                printf("Error fetching ':' in the PING command.\n");
            }
        }
        // ReleaseMutex(mutex);
    }
}

// void setup_irc(SOCKET socket,)

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

    local_addr.sin_family = AF_INET;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    if (getsockname(sockfd, (struct sockaddr *)&local_addr, &local_addr_len) == SOCKET_ERROR) {
        printf("getsockname failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // mutex = CreateMutex(NULL, FALSE, NULL);
    // if (mutex == NULL) {
    //     printf("Mutex creation failed\n");
    //     closesocket(sockfd);
    //     WSACleanup();
    //     return 1;
    // }

    HANDLE sending_thread = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);
    HANDLE receiving_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)receive_messages, (LPVOID)sockfd, 0, NULL);
    if (sending_thread == NULL || receiving_thread == NULL) {
        printf("Error creating threads\n");
        closesocket(sockfd);
        // CloseHandle(mutex);
        WSACleanup();
        return 1;
    }

    local_port = ntohs(local_addr.sin_port);

    printf("Connected to %s on port %d\n", SERVER, PORT);

    snprintf(message, sizeof(message), "NICK %s\r\n", NICKNAME);
    send_message(sockfd, message);

    snprintf(message, sizeof(message), "USER %s 0 * :%s\r\n", USERNAME, USERNAME);
    send_message(sockfd, message);

    snprintf(message, sizeof(message), "JOIN %s\r\n", CHANNEL);
    send_message(sockfd, message);

    snprintf(message, sizeof(message), "113 :%s :%d\r\n", NICKNAME, local_port);
    send_message(sockfd, message);


    // snprintf(message, sizeof(message), "/msg NickServ REGISTER %s %s\r\n", CLIENT_EMAIL, CLIENT_PASSWORD);
    // send_message(sockfd, message);

    //receive_messages(sockfd);
    WaitForSingleObject(sending_thread, INFINITE);
    WaitForSingleObject(receiving_thread, INFINITE);

    // FOR CLEANUP! DO NOT MODIFY
    closesocket(sockfd);
    // CloseHandle(mutex);
    WSACleanup();

    return 0;
}
