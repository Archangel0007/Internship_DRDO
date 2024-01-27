#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* Here the Client  has the file example.txt 

it first sends the server the name of the file it is going to send.

 next it sends the file content and finally the end marker to denote the successful file transfer.


 on the Server Side the file name is received from the client , a new file with that name is created and on 

 receiving the text it is appended to the file.
finally it is saved. */






int main() {
    
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return 1;
    }

 
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server." << std::endl;
        return 1;
    }


    std::string fileName = "example.txt";

 
    send(clientSocket, fileName.c_str(), fileName.size(), 0);

  
    std::ifstream inputFile(fileName, std::ios::binary);

    char buffer[1024];
    while (!inputFile.eof()) {
        inputFile.read(buffer, sizeof(buffer));
        int bytesRead = inputFile.gcount();
        if (bytesRead > 0) {
            send(clientSocket, buffer, bytesRead, 0);
        }
    }

  
    inputFile.close();

 
    char endMarker = '\0';
    send(clientSocket, &endMarker, 1, 0);

    // Close the socket
    close(clientSocket);

    std::cout << "File transfer completed." << std::endl;

    return 0;
}
