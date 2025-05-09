#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             // for close()
#include <arpa/inet.h>          // for inet_addr, sockaddr_in

#define PORT 8080
#define BUFFER_SIZE 4096

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET,SOCK_STREAM,0);  
    if (server_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    printf("✅ Socket created successfully.\n");
    
    // 2. Bind address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    server_addr.sin_port = htons(PORT);        // Host to Network Short

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("✅ Bind successful on port %d.\n", PORT);

     // 4. Accept a client
     client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
     if (client_fd < 0) {
         perror("Accept failed");
         close(server_fd);
         exit(EXIT_FAILURE);
     }
     printf("✅ Connection accepted.\n");

      // 5. Read data
    int bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_received < 0) {
        perror("Read failed");
    } else {
        buffer[bytes_received] = '\0';
        printf("📦 Received (%d bytes):\n%s\n", bytes_received, buffer);
    }

    // 6. Close sockets
    close(client_fd);
    close(server_fd);
    printf("🔒 Connection closed.\n");

    return 0;

}