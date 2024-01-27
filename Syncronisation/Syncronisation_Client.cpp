#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

char buffer[1024];
int client_sock;
int exit_flag = 0; // Flag to signal threads to exit

sem_t send_semaphore;

void *sendFunction(void *args) {
    while (1) {
        sem_wait(&send_semaphore); // Wait for signal to send
        if (exit_flag) {
            sem_post(&send_semaphore); // Release the semaphore
            break; // Exit the thread
        }

        fgets(buffer, sizeof(buffer), stdin);
        printf("Client: %s", buffer);
        if (strcmp(buffer, "end\n") == 0) {
            exit_flag = 1; // Set the exit flag
            sem_post(&send_semaphore); // Signal main thread
            break;
        }
        send(client_sock, buffer, sizeof(buffer), 0);

        sem_post(&send_semaphore); // Signal main thread
    }
    return NULL;
}

void *recvFunction(void *args) {
    while (1) {
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Server: %s", buffer);
        if (strcmp(buffer, "end\n") == 0) {
            exit_flag = 1; // Set the exit flag
            sem_post(&send_semaphore); // Signal send thread to exit
            break;
        }
    }
    return NULL;
}

int main() {
    pthread_t send_thread;
    pthread_t recv_thread;

    char *ip = "127.0.0.1";
    int port = 5566;

    struct sockaddr_in server_addr;
    socklen_t addr_size;

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket Error\n");
        exit(1);
    }
    printf("Tcp Client Socket Created Successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Error\n");
        exit(1);
    }
    printf("Connected to the server\n");

    sem_init(&send_semaphore, 0, 1); // Initialize the semaphore with an initial value of 1

    pthread_create(&send_thread, NULL, sendFunction, NULL);
    pthread_create(&recv_thread, NULL, recvFunction, NULL);

    pthread_join(send_thread, NULL);  // Wait for send thread to finish
    pthread_join(recv_thread, NULL);  // Wait for recv thread to finish

    close(client_sock);

    return 0;
}
