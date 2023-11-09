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
        bufsize -= i;
        buf += i;
        sum += i;
    } while (*(buf - 1) != '\n');

    return(sum);
}

int main(int argc, char** argv) {

    if(argc < 3) {
        printf("You have to pass ip address and port number\n");
        exit(1);
    }

    struct hostent *host = gethostbyname(argv[1]);
  
    int sfd, rc;
    char buf[256];
    struct sockaddr_in saddr;
    sfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        printf("Creating socket failed");
        exit(1);
    }
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    memcpy(&saddr.sin_addr.s_addr, host->h_addr, host->h_length);
    
    if (connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1) {
        printf("Connection failed");
        exit(1);
    }

    _write(sfd, "151820\n", 7);

    rc = _read(sfd, buf, sizeof(buf));
    if (rc < 0) {
        printf("Failed to read");
        exit(1);
    }

    _write(1, buf, rc);
    close(sfd);

   return 0;
}