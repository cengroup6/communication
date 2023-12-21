#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../protocols.h"

using namespace std;


int send_message_to_server(request_t req,int client_socket,struct sockaddr_in servaddr){
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer,"%d",req);
    return sendto(client_socket, buffer, strlen(buffer), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
}

// Client - server_ip port 
int main(int argc, char* argv[]) {
    int port;
    char* server_ip;
    if (argc < 3) {
        cerr << "usage: ./client <server_ip> <server_port>" << endl;
        exit(EXIT_FAILURE);
    }
    server_ip = argv[1];
    port = atoi(argv[2]);

    // Create a UDP socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Error opening client socket");
        exit(EXIT_FAILURE);
    }

    // Define the server address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); // Server port
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) { //server ip address
        cerr << "Invalid address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Listening for data from server..." << endl;

    while (true) {
        //asking for data from server
        if(send_message_to_server(SPEED,client_socket,servaddr) < 0){
            std::cerr << "sendto() failed" << std::endl;
            exit(EXIT_FAILURE);
        }


        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in senderAddr;
        socklen_t len = sizeof(senderAddr);
        // Receive data from the server
        int n = recvfrom(client_socket, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&senderAddr, &len);
        if (n < 0) {
            cerr << "Error in recvfrom" << endl;
            break;
        }

        buffer[n] = '\0';
        cout << "Received: " << buffer << endl;
    }

    // Close the socket
    close(client_socket);
    return 0;
}
