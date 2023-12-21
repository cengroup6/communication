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
#include "sensors.h"
#include "../protocols.h"


int get_rotation_data(){
    return -1;
}

int get_bpm_data(){
    return 60;
}

int get_speed_data(){
    return 100;
}

int send_message_to_client(request_t req,int sockfd,struct sockaddr_in client_addr,int cliaddr_len){
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    if (req == SPEED) {
        sprintf(buffer,"speed: %d", get_speed_data());
    }else if(req == ROTATION){
        sprintf(buffer,"rotation: %d", get_rotation_data());
    }else if(req == BPM){
        sprintf(buffer,"bpm: %d", get_bpm_data());
    }else{
        char* init = "invalid request: 404\n";
        strncpy(buffer,init,strlen(init));
        buffer[strlen(buffer)]='\0';
    }
    return sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, cliaddr_len);
}

//args - ./server ip port 
int main(int argc,char*argv[],char** envp){
    int port, sockfd, addr_size;
    struct sockaddr_in server_addr, client_addr;
    char* ip_address; 

    if(argc < 3){
        fprintf(stderr,"USAGE:./server <ip> <port>\n");
        exit(1);
    }

    //parsing ip address and port
    ip_address = argv[1];
    port = atoi(argv[2]);

    //creating server socket
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }
    //cleaning server and client address
    memset(&server_addr, 0, sizeof(server_addr));
	memset(&client_addr, 0, sizeof(client_addr));
    socklen_t cliaddr_len = sizeof(client_addr); //client address length

    //setting up server address
    server_addr.sin_family = AF_INET; //ipv4
    server_addr.sin_port = htons(port); //port of the server
    //Ip address of the server. Convert the IP address from string to binary form
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid ip address/ address not supported");
        exit(EXIT_FAILURE);
    }

    //binding socket with server address
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("server bind failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout,"server started on %s:%d\n",ip_address,port);
    
    //keep transmitting sensor data
  
    while (1) {
        char buffer_receive[1024];
        memset(buffer_receive,0,sizeof(buffer_receive));
        // Receive messages from clients
        int len = recvfrom(sockfd, buffer_receive, sizeof(buffer_receive), 0, (struct sockaddr *)&client_addr, &cliaddr_len);
        if (len < 0) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("client:%s:%d\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        buffer_receive[strlen(buffer_receive)]='\0';
        request_t req = atoi(buffer_receive);

        //sending data to client
        send_message_to_client(req,sockfd,client_addr,cliaddr_len);
    }

    close(sockfd);
    return 0;
}