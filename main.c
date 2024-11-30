#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_LEN 512
#define SERVER "irc.libera.chat"
#define PORT 6667
#define CHANNEL "#testchannel"
#define NICKNAME "TestBot"
#define USERNAME "TestBot"

int sockfd;

void format_command(const char *input, char *output) {
    if (strncmp(input, "/privmsg", 8) == 0) {
        const char *params = input + 8;
        while (*params == ' ') params++;

        char target[BUFFER_LEN];
        const char *message = strchr(params, ' ');

        if (message) {
            size_t target_len = message - params;
            strncpy(target, params, target_len);
            target[target_len] = '\0';
            message++;
            snprintf(output, BUFFER_LEN, "PRIVMSG %s :%s\r\n", target, message);
        } else {
            snprintf(output, BUFFER_LEN, "ERROR :Invalid PRIVMSG format\r\n");
        }
    } else if (strncmp(input, "/join", 5) == 0) {
        const char *channel = input + 5;
        while (*channel == ' ') channel++; // Skip leading spaces
        snprintf(output, BUFFER_LEN, "JOIN %s\r\n", channel);
    } else if (strncmp(input, "/nick", 5) == 0) {
        const char *new_nick = input + 5;
        while (*new_nick == ' ') new_nick++; // Skip leading spaces
        snprintf(output, BUFFER_LEN, "NICK %s\r\n", new_nick);
    } else {
        snprintf(output, BUFFER_LEN, "%s\r\n", input);
    }
}



void setup(int sockfd) {
    char message[BUFFER_LEN];
    printf("Connected to %s on port %d\n", SERVER, PORT);

    snprintf(message, sizeof(message), "NICK %s\r\n", NICKNAME);
    send(sockfd, message, strlen(message), 0);
    sleep(1);

    snprintf(message, sizeof(message), "USER %s 0 * :%s\r\n", USERNAME, "Test Bot Real Name");
    send(sockfd, message, strlen(message), 0);
    sleep(1);

    snprintf(message, sizeof(message), "JOIN %s\r\n", CHANNEL);
    send(sockfd, message, strlen(message), 0);
}

void send_message(int sockfd, const char *message) {
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Error sending message");
    }
    printf("Sent: %s", message);
}

void receive_messages(int sockfd) {
    char buffer[BUFFER_LEN];
    int bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        printf("Connection closed or error occurred.\n");
        exit(1);
    }
    buffer[bytesReceived] = '\0';
    printf("Received: %s", buffer);

    if (strncmp(buffer, "PING", 4) == 0) {
        char *ping_id = strchr(buffer, ':');
        if (ping_id) {
            char pong_response[BUFFER_LEN];
            snprintf(pong_response, sizeof(pong_response), "PONG %s\r\n", ping_id + 1);
            send_message(sockfd, pong_response);
        }
    }
}

int main() {
    struct sockaddr_in serverAddr;
    struct hostent *host;
    fd_set read_fds;
    char input_buffer[BUFFER_LEN];
    struct timeval timeout;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    host = gethostbyname(SERVER);
    if (!host) {
        perror("Error resolving hostname");
        close(sockfd);
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    memcpy(&serverAddr.sin_addr, host->h_addr_list[0], host->h_length);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    setup(sockfd);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("Select failed");
            break;
        }

        if (FD_ISSET(sockfd, &read_fds)) {
            receive_messages(sockfd);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(input_buffer, BUFFER_LEN, stdin) != NULL) {
                input_buffer[strcspn(input_buffer, "\n")] = '\0';
                if (strlen(input_buffer) > 0) {
                    char formatted_msg[BUFFER_LEN + 2];
                    if (strcmp(input_buffer, "/quit") == 0) {
                        printf("Quitting...\n");
                        break;
                    }
                    if (input_buffer[0] == '/') {
                        format_command(input_buffer, formatted_msg);
                    } else {
                        snprintf(formatted_msg, sizeof(formatted_msg), "PRIVMSG %s :%s\r\n", CHANNEL, input_buffer);
                    }
                    send_message(sockfd, formatted_msg);
                }
            }
        }
    }

    close(sockfd);
    return 0;
}

