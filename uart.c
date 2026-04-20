#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/select.h>

#define UART_PORT "/dev/ttyUSB0"
#define BAUDRATE B115200

int main() {
    int fd;
    struct termios tty;

    fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Failed to open UART port");
        return EXIT_FAILURE;
    }

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // Set baud rate
    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    // 8N1 configuration, local connection, enable read
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    tty.c_cflag |= (CLOCAL | CREAD);            
    tty.c_cflag &= ~(PARENB | CSTOPB | CRTSCTS); 
    
    // Raw mode: disable canonical processing, echoing, and signals
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // Disable software flow control and special char handling
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~(OPOST | ONLCR);

    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // Transmit phase
    const char *msg = "ACT FW Runner Init\r\n";
    if (write(fd, msg, strlen(msg)) < 0) {
        perror("UART write error");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("Tx successful.\n");

    // Receive phase with 5s timeout
    fd_set read_fds;
    struct timeval tv = {5, 0}; // 5 sec, 0 microsec
    char rx_buf[256] = {0};

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    printf("Waiting for Rx...\n");
    int ret = select(fd + 1, &read_fds, NULL, NULL, &tv);
    
    if (ret < 0) {
        perror("select failed");
    } else if (ret == 0) {
        printf("Rx timeout.\n");
    } else {
        int n = read(fd, rx_buf, sizeof(rx_buf) - 1);
        if (n > 0) {
            printf("Received %d bytes: %s\n", n, rx_buf);
        } else {
            perror("UART read error");
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}
