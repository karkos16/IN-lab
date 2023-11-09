#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

int _write(int sfd, char *buf, int len) {
    while (len > 0) {
        int i = write(sfd, buf, len);
        len -= i;
        buf += i;
    }
}

int _read(int sfd, char *buf, int bufsize) {
    int i;
    int sum = 0;
    do {
        i = read(sfd, buf, bufsize);
        if (i < 0) {
            return(-1);
        }
        bufsize -= i;
        buf += i;
        sum += i;
    } while (*(buf - 1) != '\n');

    return(sum);
}

int main(int argc, char** argv) {
    int sfd, cfd;
    int on = 1;
    socklen_t sl;
    struct sockaddr_in saddr, caddr;

    char indeks[256];

    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("150.254.32.67");
    saddr.sin_port = htons(1234);

    sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sfd == -1) {
        printf("Creating socket failed");
        exit(1);
    }
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

    if(bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr)) == -1) {
        printf("Bind failed\n");
    }
    
    if (listen(sfd,10) == -1) {
        printf("Listen failed\n");
        exit(1);
    }

    while(1) {
        sl = sizeof(caddr);

        cfd = accept(sfd, (struct sockaddr*) &caddr, &sl);

        printf("new connection from %s:%d\n",
            inet_ntoa((struct in_addr)caddr.sin_addr),
            ntohs(caddr.sin_port)
        );

        _read(cfd, indeks, 256);

        printf(indeks);
        if (strncmp("151820", indeks, 6) == 0) { 
            _write(cfd, "Karol Kostrubiec\n", 17);
        }
        else if (strncmp(indeks, "151843", 6) == 0) {
            _write(cfd, "Mikolaj Diakowski\n", 18);
        } else {
            _write(cfd, "Numer ideksu nieznany\n", 22);
        }
        close(cfd);
    }
}