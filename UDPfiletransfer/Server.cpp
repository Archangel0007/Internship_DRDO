#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 65536

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Set up server address structure
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        return -1;
    }

    // Receive image request from client
    char buffer[MAX_BUFFER_SIZE];
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    ssize_t recvSize = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (recvSize == -1) {
        perror("Error receiving data");
        close(serverSocket);
        return -1;
    }

    buffer[recvSize] = '\0';

    if (strcmp(buffer, "send image") == 0) {
        // Read image file
        std::ifstream imageFile("sample_image.jpg", std::ios::binary | std::ios::ate);
        if (!imageFile.is_open()) {
            std :: cout<<"hello world";
            perror("Error opening image file");
            close(serverSocket);
            return -1;
        }

        // Get file size
        std::streamsize imageSize = imageFile.tellg();
        imageFile.seekg(0, std::ios::beg);

        // Read image data into buffer
        char* imageData = new char[imageSize];
        imageFile.read(imageData, imageSize);
        imageFile.close();
        std :: cout<<"HelLo world";
        // Send image data to client
        sendto(serverSocket, imageData, imageSize, 0,reinterpret_cast<struct sockaddr*>(&clientAddr), clientAddrLen);
        std :: cout<<"Hello World";
        delete[] imageData;
    }

    close(serverSocket);
    return 0;
}
