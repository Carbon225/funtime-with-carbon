#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

int main(int argc, char **argv)
{
    int netsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (netsock < 0)
    {
        perror("Could not create socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(netsock, (struct sockaddr*) &addr, sizeof addr) != 0)
    {
        perror("Could not bind socket");
        close(netsock);
        return -1;
    }

    printf("Socket opened on 0.0.0.0:%d\n", ntohs(addr.sin_port));

    close(netsock);

    return 0;
}
