#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(void) {
    const char *device = "/dev/input/mice";
    int fd;
    signed char data[3];
    int x = 0, y = 0;

    fd = open(device, O_RDONLY);
    if (fd == -1) {
        perror("Error opening /dev/input/mice");
        return 1;
    }

    printf("Tracking mouse movement...\n");

    while (1) {
        ssize_t bytes = read(fd, data, sizeof(data));
        if (bytes > 0) {
            // data[0] has button states
            int dx = (int)data[1];
            int dy = (int)data[2];
            x += dx;
            y -= dy; // invert Y to match screen coordinates

            printf("\rX = %d, Y = %d  ", x, y);
            fflush(stdout);
        } else {
            perror("Read error");
            break;
        }
    }

    close(fd);
    return 0;
}