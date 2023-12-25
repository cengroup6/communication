#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../protocols.h"

using namespace std;


// request_t parse_requested_data(char* data){
//     if(strncmp(data,"rmp",strlen("rmp"))==0){
//         return SPEED;
//     }else if(strncmp(data,"rotation",strlen("rotation"))==0){
//         return ROTATION;
//     }else if(strncmp(data,"bpm",strlen("bpm"))==0){
//         return BPM;
//     }else{
//         return INVALID;
//     }
// }

int send_message_to_server(request_t req,int sockfd,struct sockaddr_in servaddr){
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer,"%d",req);
    return sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
}

// Client - <server_ip> <port> <interested_data> 
int main(int argc, char* argv[]) {
    int port;
    char* server_ip;
    struct sockaddr_in servaddr;

    if (argc < 3) {
        cerr << "usage: ./client <server_ip> <server_port>" << endl;
        exit(EXIT_FAILURE);
    }
    server_ip = argv[1];
    port = atoi(argv[2]);
    

    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening client socket");
        exit(EXIT_FAILURE);
    }

    // Define the server address and port
    memset(&servaddr, 0, sizeof(servaddr));
    socklen_t len = sizeof(servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); // Server port
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) { //server ip address
        cerr << "Invalid address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    while(1){
        if(send_message_to_server(INIT,sockfd,servaddr) < 0){
            std::cerr << "send_message_to_server(sendto) failed" << std::endl;
            exit(EXIT_FAILURE);
        }else{
            break;
        }
    }

    cout << "Listening for data from server..." << endl;
    
    char buffer[1024];
    while (true) {        
        memset(buffer, 0, sizeof(buffer));
        // Receive data from the server
        int bytes_received;
        if((bytes_received = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, &len))<0){
            cerr << "Error in recvfrom" << endl;
            break;
        }
        buffer[bytes_received] = '\0';
        cout<< buffer;
    }
    // Close the socket
    close(sockfd);
    return 0;
}
