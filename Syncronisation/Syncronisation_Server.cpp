#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dispatch/dispatch.h>

pthread_t send_thread;
pthread_t recv_thread;
char buffer[1024];
int server_sock, client_sock;

int exit_flag = 0; // Flag to signal threads to exit

void *sendFunction(void *args) {
    dispatch_semaphore_t send_semaphore = (dispatch_semaphore_t)args;
    while (1) {
        dispatch_semaphore_wait(send_semaphore, DISPATCH_TIME_FOREVER); // Wait for recv to complete
        if (exit_flag) {
            dispatch_semaphore_signal(send_semaphore); // Release the semaphore
            return NULL; // Exit the thread
        }

        fgets(buffer, sizeof(buffer), stdin);
        printf("Server: %s", buffer);
        send(client_sock, buffer, sizeof(buffer), 0);

        dispatch_semaphore_signal(send_semaphore); // Signal that send is done
    }
    return NULL;
}

void *recvFunction(void *args) {
    dispatch_semaphore_t send_semaphore = (dispatch_semaphore_t)args;
    while (1) {
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Client: %s", buffer);
        if (strcmp(buffer, "end\n") == 0) {
            exit_flag = 1; // Set the exit flag
            dispatch_semaphore_signal(send_semaphore); // Signal send thread to exit
            return NULL;
        }
        dispatch_semaphore_signal(send_semaphore); // Signal that recv is done
    }
    return NULL;
}

int main() {
    dispatch_semaphore_t send_semaphore = dispatch_semaphore_create(1);

    int port = 5566;

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket Error\n");
        exit(1);
    }
    printf("Tcp Server Socket Created Successfully\n");

    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding Error\n");
        exit(1);
    }

    if (listen(server_sock, 1) < 0) {
        perror("Listening Error\n");
        exit(1);
    }

    printf("Server listening on port %d...\n", port);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_sock < 0) {
        perror("Accepting connection failed\n");
        close(server_sock);
        exit(1);
    }

    printf("Connected to the client\n");

    pthread_create(&recv_thread, NULL, recvFunction, send_semaphore);
    pthread_create(&send_thread, NULL, sendFunction, send_semaphore);

    pthread_join(recv_thread, NULL); // Wait for recv thread to finish

    close(client_sock);
    close(server_sock);

    return 0;
}
