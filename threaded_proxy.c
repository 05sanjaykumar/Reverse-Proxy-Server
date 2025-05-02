#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 8080
#define BACKEND_PORT 8000
#define BUFFER_SIZE 4096

void* handle_client(void* arg)
{
    int client_fd = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE] = {0};

    // 1. Read request from client
    int bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);
    if (bytes_received <= 0) {
        perror("Read from client failed");
        close(client_fd);
        return NULL;
    }

    buffer[bytes_received] = '\0';
    printf("📦 Received (%d bytes):\n%s\n", bytes_received, buffer);

    // 2. Connect to backend
    int backend_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (backend_fd == -1) {
        perror("Backend socket creation failed");
        close(client_fd);
        return NULL;
    }

    struct sockaddr_in backend_addr;
    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(BACKEND_PORT);
    backend_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(backend_fd, (struct sockaddr*)&backend_addr, sizeof(backend_addr)) < 0) {
        perror("Backend connection failed");
        close(client_fd);
        close(backend_fd);
        return NULL;
    }

    printf("✅ Connected to backend server.\n");

    // 3. Forward request to backend
    write(backend_fd, buffer, bytes_received);
    printf("📤 Forwarded request to backend.\n");

    // 4. Read response from backend
    char *backend_buffer = malloc(BUFFER_SIZE);
    if (!backend_buffer) {
        perror("Malloc failed");
        close(client_fd);
        close(backend_fd);
        return NULL;
    }

    int total_read = 0, n;
    while ((n = read(backend_fd, backend_buffer + total_read, BUFFER_SIZE - total_read - 1)) > 0) {
        total_read += n;
        if (total_read >= BUFFER_SIZE - 1) break;
    }
    backend_buffer[total_read] = '\0';

    printf("📥 Received from backend (%d bytes):\n%s\n", total_read, backend_buffer);

    // 5. Send response back to client
    write(client_fd, backend_buffer, total_read);
    printf("📤 Sent response back to client.\n");

    // 6. Cleanup
    free(backend_buffer);
    close(backend_fd);
    close(client_fd);
    return NULL;
}

int main()
{
    int server_fd;
    struct sockaddr_in server_addr;
    socklen_t client_len;
    struct sockaddr_in client_addr;

    // 1. Create socket

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("✅ Socket created successfully.\n");

    // 2. Bind
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("✅ Bind successful on port %d.\n", PORT);

    // 3. Listen

    if (listen(server_fd, 10) < 0) {
        perror("Listening Failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while(1){
        int* client_fd =(int*)malloc(sizeof(int));
        client_len = sizeof(client_addr);
        *client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (*client_fd < 0) {
            perror("Accept failed");
            free(client_fd);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client_fd) != 0)
        {
            perror("Thread creation failed");
            close(*client_fd);
            free(client_fd);
        }
        else
        {
            pthread_detach(tid); 
        }
    }
    close(server_fd);
    return 0;
}