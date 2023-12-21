#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "server_config.h"



//args - ./server ip port 
int main(int argc,char*argv[],char** envp){
    int port, serversock, addr_size;
    struct sockaddr_in server_addr, client_addr;
    char* ip_address; 

    if(argc < 3){
        fprintf(stderr,"USAGE:./server <ip> <port>");
        exit(1);
    }

    //parsing ip address and port
    ip_address = argv[1];
    port = atoi(argv[2]);

    printf("ip:%s, port:%d\n",ip_address,port);

    //creating server socket
    if((serversock = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }
    //cleaning server and client address
    memset(&server_addr, 0, sizeof(server_addr));
	memset(&client_addr, 0, sizeof(client_addr));

    //setting up server address
    server_addr.sin_family = AF_INET; //ipv4
    server_addr.sin_port = htons(port); //port of the server
    //Ip address of the server. Convert the IP address from string to binary form
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid ip address/ address not supported");
        exit(EXIT_FAILURE);
    }

    //binding socket with server address
    if (bind(serversock, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("server bind failed");
        exit(EXIT_FAILURE);
    }

    
    //receiving from clinet
    


    close(serversock);

    return 0;
}