#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int fd, rc, on = 1;
    socklen_t sl;
    char buf[1024];
    struct sockaddr_in sa;  
    
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_BROADCAST;
    sa.sin_port = htons(atoi(argv[2]));
    fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    sendto(fd, "Karol Kostrubiec\n", 17, 0, (struct sockaddr*)&sa, sizeof(sa));
    sl = sizeof(sa);
    while(1) {
        rc = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sa, &sl);

        write(1, buf, rc);
    }
        
    
    
    close(fd);
}