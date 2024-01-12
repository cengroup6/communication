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
#include "sensors.h"


int send_message_to_client(int sockfd,char* data, struct sockaddr_in client_addr,int cliaddr_len){
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"%s", data);
    return sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, cliaddr_len);
}

//args - ./server ip server_port 
int main(int argc,char*argv[],char** envp){
    int server_port,client_port, sockfd, addr_size;
    struct sockaddr_in server_addr, client_addr;
    char* server_ip_address;
    char* client_ip_address; 

    if(argc < 4){
        fprintf(stderr,"USAGE:./server <server_ip> <server_port> <client_ip>\n");
        exit(1);
    }

    //setting up sensor 
    int sensorfd = openSerialPort("/dev/ttyUSB0"); // Change to match your server_port
    if(sensorfd < 0){
        sensorfd = openSerialPort("/dev/ttyUSB1"); // Change to match your server_port
        if(sensorfd < 0){
            perror("failed opening sensor serial server_port");
            exit(EXIT_FAILURE);
        }
    }
    configureSerialPort(sensorfd);

    //parsing ip address and server_port
    server_ip_address = argv[1];
    server_port = atoi(argv[2]);
    client_ip_address = argv[3];
    client_port = 54000;

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
    server_addr.sin_port = htons(server_port); //server_port of the server
    //Ip address of the server. Convert the IP address from string to binary form
    if (inet_pton(AF_INET, server_ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid ip address/ address not supported");
        exit(EXIT_FAILURE);
    }

    //setting client ip address
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(client_port);
    //Ip address of the client. Convert the IP address from string to binary form
    if (inet_pton(AF_INET, client_ip_address, &client_addr.sin_addr) <= 0) {
        perror("Invalid ip address/ address not supported");
        exit(EXIT_FAILURE);
    }

    //binding socket with server address
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("server bind failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout,"server started on %s:%d\n",server_ip_address,server_port);
    

    char buffer_receive[1024];
    char sensor_data[1024];
    while (1) {
        //read sensor data
        memset(sensor_data,0,sizeof(sensor_data));
        int sensor_read_bytes = read(sensorfd, sensor_data, sizeof(sensor_data));
        if (sensor_read_bytes < 0) {
            perror("sensor data read failed");
            break;
        }
        sensor_data[sensor_read_bytes]='\0';
        //sending data to active clients
        send_message_to_client(sockfd,sensor_data,client_addr,cliaddr_len);
    }

    close(sensorfd);
    close(sockfd);
    return 0;
}
