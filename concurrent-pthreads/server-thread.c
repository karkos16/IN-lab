#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

struct cln {
    int cfd;
    struct sockaddr_in caddr;
};

void* cthread(void* arg) {
    struct cln* c = (struct cln*)arg;
    char indeks[256];
    printf("[%lu] new connection from: %s:%d\n", (unsigned long int)pthread_self(),
        inet_ntoa((struct in_addr)c->caddr.sin_addr),
        ntohs(c->caddr.sin_port)
    );
    
    read(c->cfd, indeks, 256);
    if (strncmp(indeks,"151820", 6) == 0) { 
        write(c->cfd, "Karol Kostrubiec\n", 17);
    }
    else if (strncmp(indeks, "151843", 6) == 0) {
        write(c->cfd, "Mikolaj Diakowski\n", 18);
    } else {
        write(c->cfd, "Numer ideksu nieznany\n", 25);
    }

    close(c->cfd);
    free(c);
}

int main(int argc, char** argv) {
    pthread_t tid;
    int sfd, cfd;
    int on = 1;
    socklen_t sl;
    struct sockaddr_in saddr, caddr;

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
        struct cln* c = malloc(sizeof(struct cln));
        sl = sizeof(caddr);
        
        c->cfd = accept(sfd, (struct sockaddr*)c->&caddr, &sl);
        pthread_create(&tid, NULL, cthread, c);
        pthread_detach(tid);
    }
}