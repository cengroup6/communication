#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "server_config.h"
#include "sensors.h"
#include "../protocols.h"

// Globals
int sockfd, sensorfd;
struct sockaddr_in target_addr; // Target client address

int send_message_to_client(int sockfd, char* data, struct sockaddr_in addr, int addr_len) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%s", data);
    return sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, addr_len);
}

// args - ./server ip port target_ip target_port
int main(int argc, char*argv[], char** envp) {
    int port, target_port, addr_size;
    struct sockaddr_in server_addr;
    char* ip_address, *target_ip;

    if (argc < 5) {
        fprintf(stderr, "USAGE: ./server <ip> <port> <target_ip> <target_port>\n");
        exit(1);
    }

    // Setting up sensor
    sensorfd = openSerialPort("/dev/ttyUSB0"); // Change to match your port
    if (sensorfd < 0) {
        perror("failed opening sensor serial port");
        exit(EXIT_FAILURE);
    }
    configureSerialPort(sensorfd);

    // Parsing ip address, port, target_ip, and target_port
    ip_address = argv[1];
    port = atoi(argv[2]);
    target_ip = argv[3];
    target_port = atoi(argv[4]);

    // Creating server socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    // Setting up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // ipv4
    server_addr.sin_port = htons(port); // port of the server
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address/ address not supported");
        exit(EXIT_FAILURE);
    }

    // Binding socket with server address
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("server bind failed");
        exit(EXIT_FAILURE);
    }

    // Setting up target client address
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET; // ipv4
    target_addr.sin_port = htons(target_port); // target port
    if (inet_pton(AF_INET, target_ip, &target_addr.sin_addr) <= 0) {
        perror("Invalid target IP address/ address not supported");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "server started on %s:%d\n", ip_address, port);
    fprintf(stdout, "target client %s:%d\n", target_ip, target_port);

    char sensor_data[1024];
    while (1) {
        // Reading sensor data
        memset(sensor_data, 0, sizeof(sensor_data));
        int sensor_read_bytes = read(sensorfd, sensor_data, sizeof(sensor_data));
        if (sensor_read_bytes < 0) {
            perror("sensor data read failed");
            break;
        }
        sensor_data[sensor_read_bytes] = '\0';

        // Sending data to target client
        if (send_message_to_client(sockfd, sensor_data, target_addr, sizeof(target_addr)) < 0) {
            perror("Failed to send message to target client");
        }
    }

    close(sensorfd);
    close(sockfd);
    return 0;
}

