#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 8080
#define BACKEND_PORT 8000
#define BUFFER_SIZE 4096

void* handle_client(void* arg){
    
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