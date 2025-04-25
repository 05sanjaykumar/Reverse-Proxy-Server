#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             // for close()
#include <arpa/inet.h>          // for inet_addr, sockaddr_in

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    // 3. Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("👂 Listening for connections...\n");

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

    // 6. Connect to backend (localhost:8000)

    int backend_fd;
    struct sockaddr_in backend_addr;

    backend_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (backend_fd == -1){
        perror("Backend socket creation failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(8000);
    backend_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    if (connect(backend_fd, (struct sockaddr*)&backend_addr, sizeof(backend_addr)) < 0) {
        perror("Backend connection failed");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("✅ Connected to backend server.\n");

    // 7. Forward the request to the backend

    write(backend_fd, buffer, bytes_received);  
    printf("📤 Forwarded request to backend.\n");

    // 8. Read response from backend

    char *backend_buffer = malloc(BUFFER_SIZE);
    if (!backend_buffer) {
        perror("malloc failed");
        close(backend_fd);
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    int backend_bytes = read(backend_fd, backend_buffer, BUFFER_SIZE - 1);
    backend_buffer[backend_bytes] = '\0';

    printf("📥 Received from backend (%d bytes):\n%s\n", backend_bytes, backend_buffer);


    // 9. Send response back to original client
    write(client_fd, backend_buffer, backend_bytes);
    printf("📤 Sent response back to client.\n");

    // 10. Cleanup
    free(backend_buffer);
    close(backend_fd);

    close(client_fd);


    return 0;
}
