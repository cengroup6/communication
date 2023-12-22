#ifndef SENSORS_H
#define SENSORS_H



#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

int openSerialPort(const char *portName) {
    int fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("openSerialPort: Unable to open port ");
    } else {
        fcntl(fd, F_SETFL, 0);
    }
    return fd;
}

void configureSerialPort(int fd) {
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);
}

// int main() {
//     int fd = openSerialPort("/dev/ttyUSB0"); // Change to match your port
//     if (fd == -1) return -1;

//     configureSerialPort(fd);

//     char buffer[256];
//     while (1) {
//         int n = read(fd, buffer, sizeof(buffer) - 1);
//         if (n > 0) {
//             buffer[n] = 0;
//             printf("Received: %s\n", buffer);
//         }
//     }

//     close(fd);
//     return 0;
// }




#endif