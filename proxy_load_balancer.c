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

    if (listen(server_fd, 10) < 0) {  // 10 is the backlog queue size
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue; // try next connection instead of exiting
        }
        printf("✅ Connection accepted.\n");
    
        // 5. Read data
        int bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_received < 0) {
            perror("Read failed");
            close(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("📦 Received (%d bytes):\n%s\n", bytes_received, buffer);
    
        // 6. Select backend and connect
        int backend_fd;
        struct sockaddr_in backend_addr;
        backend_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (backend_fd == -1) {
            perror("Backend socket creation failed");
            close(client_fd);
            continue;
        }
    
        // Backend ports and round-robin
        int backend_ports[] = {8000, 8001, 8002};
        int backend_count = sizeof(backend_ports) / sizeof(backend_ports[0]);
        static int current_backend = 0;
        int selected_port = backend_ports[current_backend];
        current_backend = (current_backend + 1) % backend_count;
    
        backend_addr.sin_family = AF_INET;
        backend_addr.sin_port = htons(selected_port);
        backend_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
        printf("➡️ Routing to backend on port %d\n", selected_port);
    
        if (connect(backend_fd, (struct sockaddr*)&backend_addr, sizeof(backend_addr)) < 0) {
            perror("Backend connection failed");
            close(client_fd);
            close(backend_fd);
            continue;
        }
        printf("✅ Connected to backend server.\n");
    
        // Forward request and relay response
        write(backend_fd, buffer, bytes_received);
        printf("📤 Forwarded request to backend.\n");
    
        char *backend_buffer = malloc(BUFFER_SIZE);
        if (!backend_buffer) {
            perror("malloc failed");
            close(backend_fd);
            close(client_fd);
            continue;
        }
    
        int total_read = 0, n;
        while ((n = read(backend_fd, backend_buffer + total_read, BUFFER_SIZE - total_read - 1)) > 0) {
            total_read += n;
            if (total_read >= BUFFER_SIZE - 1) break;
        }
        backend_buffer[total_read] = '\0';
    
        printf("📥 Received from backend (%d bytes):\n%s\n", total_read, backend_buffer);
    
        write(client_fd, backend_buffer, total_read);
        printf("📤 Sent response back to client.\n");
    
        // Cleanup for this connection
        free(backend_buffer);
        close(backend_fd);
        close(client_fd);
    }
    return 0;

}
