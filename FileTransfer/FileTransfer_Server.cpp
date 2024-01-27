#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
   
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }


    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket." << std::endl;
        return 1;
    }

    
    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error listening for connections." << std::endl;
        return 1;
    }

    std::cout << "Server is listening for incoming connections..." << std::endl;

    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

 
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error accepting client connection." << std::endl;
        return 1;
    }

   
    char fileName[256];
    memset(fileName, 0, sizeof(fileName));
    recv(clientSocket, fileName, sizeof(fileName), 0);

   
    std::ofstream outputFile(fileName, std::ios::binary);

  
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        outputFile.write(buffer, bytesRead);
    }

  
    outputFile.close();

 
    char endMarker;
    recv(clientSocket, &endMarker, 1, 0);

    
    close(clientSocket);
    close(serverSocket);

    std::cout << "File transfer completed." << std::endl;

    return 0;
}
