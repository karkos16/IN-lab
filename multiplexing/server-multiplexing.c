#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

int main(int argc, char** argv) {
    socklen_t slt;
    int on = 1;
    int sfd, cfd, fdmax, fda, rc, i;
    struct sockaddr_in saddr, caddr;
    static struct timeval timeout;
    fd_set mask, rmask, wmask;
    fd_set nameA, nameB;
    char indeks[6];

    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
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

    FD_ZERO(&mask);
    FD_ZERO(&rmask);
    FD_ZERO(&wmask);
    FD_ZERO(&nameA);
    FD_ZERO(&nameB);
    fdmax = sfd;

    while(1) {
        FD_SET(sfd, &mask);
        rmask = mask;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        rc = select(fdmax+1, &rmask, &wmask, (fd_set*)0, &timeout);
        if (rc == 0) continue;
        fda = rc;
        if (FD_ISSET(sfd, &rmask)) {
            fda -= 1;
            slt = sizeof(caddr);
            cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);
            FD_SET(cfd, &mask);
            if (cfd > fdmax) fdmax = cfd;
        }
        
        for (i = sfd+1; i <= fdmax && fda > 0; i++) {

            if (FD_ISSET(i, &wmask)) {
                fda -= 1;
                if(FD_ISSET(i, &nameA)) {
                    write(i, "Karol Kostrubiec\n", 17);
                    FD_CLR(i, &nameA);
                } 
                else {
                    if(FD_ISSET(i, &nameB)) {
                        write(i, "Mikolaj Diakowski\n", 18);
                        FD_CLR(i, &nameB);
                    }  else {
                        write(i, "Numer ideksu nieznany\n", 23);
                    }
                }
               
                close(i);
                FD_CLR(i, &wmask);
                FD_CLR(i, &mask);
                
                if (i == fdmax) {
                    while(fdmax > sfd && !FD_ISSET(fdmax, &mask)) {
                        fdmax -= 1;
                    }
                }
            }

            if(FD_ISSET(i, &rmask)) {
                read(i, indeks, 16);
                if (strncmp("151820", indeks, 6) == 0) { 
                    FD_SET(i, &nameA);
                }
                else if (strncmp(indeks, "151843", 6) == 0) {
                    FD_SET(i, &nameB);
                }
                FD_CLR(i, &mask);
                FD_SET(i, &wmask);
            }
        }
    }
}