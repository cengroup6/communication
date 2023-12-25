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
#include <sys/time.h>
#include "server_config.h"
#include "sensors.h"
#include "../protocols.h"

#define MAX_CLIENT 5
#define FAILED_MSG_THRESHOLD 4

//globals
int client_count = 0,sockfd,sensorfd;

struct Client {
    struct sockaddr_in client_addr;
    int failed_msg_count;
    int active; // Indicates if the client slot is active
} clients[MAX_CLIENT];


int add_client(struct sockaddr_in new_client_addr) {
    // Check if the client already exists
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && memcmp(&clients[i].client_addr, &new_client_addr, sizeof(new_client_addr)) == 0) {
            return -1; // Client already exists
        }
    }

    // Find an empty slot and add the new client
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clients[i].active) {
            clients[i].client_addr = new_client_addr;
            clients[i].active = 1;
            clients[i].failed_msg_count = 0; 
            client_count++;
            return i; // Return the index where the client was added
        }
    }
    return -1; // No empty slot found
}

void remove_client(struct sockaddr_in client_addr) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        // Check if the client's address matches
        if (clients[i].active && memcmp(&clients[i].client_addr, &client_addr, sizeof(client_addr)) == 0) {
            clients[i].active = 0; // Mark the client as inactive
            client_count--;
            return;
        }
    }
}

int send_message_to_client(int sockfd,char* data, struct sockaddr_in client_addr,int cliaddr_len){
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"%s", data);
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

    //setting up sensor 
    sensorfd = openSerialPort("/dev/ttyUSB0"); // Change to match your port
    if(sensorfd < 0){
        perror("failed opening sensor serial port");
        exit(EXIT_FAILURE);
    }
    configureSerialPort(sensorfd);

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
    

    char buffer_receive[1024];
    char sensor_data[1024];
    while (1) {
        memset(buffer_receive,0,sizeof(buffer_receive));
        //making receive non-blocking
        struct timeval timeout;
        timeout.tv_sec = 0;  // 0 seconds
        timeout.tv_usec = 100000;  // 100 milliseconds (100000 microseconds)

        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            perror("setsockopt failed");
        }
        //waiting only for 100 milliseconds.
        int byte_received = recvfrom(sockfd, buffer_receive, sizeof(buffer_receive), 0, (struct sockaddr *)&client_addr, &cliaddr_len);
        if (byte_received < 0) {
            if (errno == EWOULDBLOCK) {
                // No data received within the timeout period
            } else {
                perror("recvfrom");
                break;
            }
        }else{ //new client connection
            buffer_receive[byte_received]='\0'; //this is client message
            printf("client msg:%d\n",atoi(buffer_receive));
            if(add_client(client_addr)!=-1){
                printf("client:%s:%d is in active client list\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            }
        }

        //reading sensor data
        memset(sensor_data,0,sizeof(sensor_data));
        int sensor_read_bytes = read(sensorfd, sensor_data, sizeof(sensor_data));
        if (sensor_read_bytes < 0) {
            perror("sensor data read failed");
            break;
        }
        sensor_data[sensor_read_bytes]='\0';

        //should add the client in active client list
        int clients_removed=0;
        for(int i = 0; i < client_count; i++){
            if(send_message_to_client(sockfd,sensor_data,clients[i].client_addr,cliaddr_len)<0){
                printf("client:%s missed data\n",inet_ntoa(clients[i].client_addr.sin_addr));
                clients[i].failed_msg_count+=1;
            }else{ //resetting failed message count
                clients[i].failed_msg_count=0;
            }
            //removing client if failed msg reach threshold
            if(clients[i].failed_msg_count > FAILED_MSG_THRESHOLD){
                printf("suspected dead client being removed:%s\n",inet_ntoa(clients[i].client_addr.sin_addr));
                clients[i].active=0;
                clients_removed+=1;
            }
        }
        client_count-=clients_removed;
    }

    close(sensorfd);
    close(sockfd);
    return 0;
}