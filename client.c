#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8080       // Port to connect to
#define SERVER_IP "127.0.0.1"  // Server IP address (localhost)
#define BUFFER_SIZE 1024       // Buffer size for messages

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // Step 1: Create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");

    // Step 2: Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert server IP address to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Step 3: Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to the server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // Step 4: Communicate with the server
    while (1) {
        printf("Enter message to send (type 'exit' to quit): ");
        fgets(message, BUFFER_SIZE, stdin);

        // Remove newline character from input
        message[strcspn(message, "\n")] = '\0';

        // Exit condition
        if (strcmp(message, "exit") == 0) {
            printf("Closing connection...\n");
            break;
        }

        // Send the message to the server
        send(client_fd, message, strlen(message), 0);

        // Receive the server's response (optional, for echo servers)
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            printf("Server response: %s\n", buffer);
        } else if (bytes_received == 0) {
            printf("Server closed the connection.\n");
            break;
        } else {
            perror("Error receiving data");
            break;
        }
    }

    // Step 5: Close the socket
    close(client_fd);
    printf("Client shut down.\n");
    return 0;
}

