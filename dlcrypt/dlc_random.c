#include "dlc_base.h"

#define DLC_URANDOM_DIVICE "/dev/urandom"

int dlc_urandom(char *buf, int len)
{
    int fd, n;
    
    fd = open(DLC_URANDOM_DIVICE, O_RDONLY);
    if(fd == -1) return -1;
    
    n = read(fd, buf, len);
    if(n == -1) return -1;
    
    close(fd);
    return n;
}