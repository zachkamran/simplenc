#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>


int handle_session(int session, int client);
int init_connect(char *host,char *port,int udp);
int read_socket(int sockfd);
int server_listen(char *host, char* port, int udp);
int send_stdin(int sockfd);
void *read_socket_client(void* arg);
void *read_stdin_server(void *arg);

int handle_session(int sockfd, int client)
{
    if (client) {
        send_stdin(sockfd);
        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, read_socket_client, &sockfd)!=0){
            fprintf(stderr, "error in handle session pthread create");
            exit(1);
       }
    }else {
        read_socket(sockfd);
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, read_stdin_server, &sockfd) != 0) {
            fprintf(stderr, "error in handle session pthread create");
            exit(1);
        }
    }

    return 1;
}

void *read_socket_client(void* arg)
{
    int sockfd = (int)arg;
    char data[2048];
    int len;
    while(1){
        memset(&data,0, sizeof(data));
        len = read(sockfd, data, sizeof(data) -1);


        if (!len){
            close(sockfd);
            exit(0);
        }

        fprintf(stdout,"%s", data);
    }

}



int init_connect(char *host,char *port,int udp){
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    if (udp) {
        hints.ai_socktype = SOCK_DGRAM; // for UDP
    }else {
        hints.ai_socktype = SOCK_STREAM; // for TCP
    }
    if ( getaddrinfo(host,port, &hints, &servinfo) !=0) {
        fprintf(stderr, "internal error in TCP get addr info\n");
            exit(1);
        }

    // loop through all results and connect to thd first we can
    for(p = servinfo; p!=NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }
    if (p==NULL){
        fprintf(stderr, "internal error looped off end of list");
        exit(2);
    }


    handle_session(sockfd, 1);
    return 1;

}


void *read_stdin_server(void *arg){
// arg is the socket to send on
    int sockfd = (int)arg;
    char buf[2048];
    char* input;
    while(1) {
        memset(&buf, 0, sizeof(buf));
        input = fgets(buf, sizeof(buf), stdin);
        if (!input) {
            close(sockfd);
            exit(0);
        }

        write(sockfd, buf, sizeof(buf));


    }
}

int read_socket(int sockfd){
    char data[2048];
    int len;
    while(1){
        memset(&data,0, sizeof(data));
        len = read(sockfd, data, sizeof(data) -1);


        if (!len){
            close(sockfd);
            exit(0);
        }

        fprintf(stdout,"%s", data);
    }
    return 1;

}

int send_stdin(int sockfd){
// arg is the socket to send on
    char buf[2048];
    char* input;
    while(1) {
        memset(&buf, 0, sizeof(buf));
        input = fgets(buf, sizeof(buf), stdin);
        if (!input) {
            close(sockfd);
            exit(0);
        }

        write(sockfd, buf, sizeof(buf));


    }
}

int server_listen(char *host, char* port, int udp)
{
    const char* portname= port ;
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    const char* hostname;
    if (host)
        hostname= host; /* wildcard */

    else {
        hostname = 0;
        hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
    }

    hints.ai_family = AF_INET;
    hints.ai_protocol=0;

    if(udp)
        hints.ai_socktype = SOCK_DGRAM;
    else
        hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res=0;
    int err=getaddrinfo(hostname,portname,&hints,&res);
    if (err!=0) {
        fprintf(stderr, "internal error in server listen get addrinfo");
        exit(1);
    }
    int server_fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if (server_fd==-1) {
        fprintf(stderr, "internal error in server listen socket");
        exit(1);
    }
    if (bind(server_fd,res->ai_addr,res->ai_addrlen)==-1) {
        fprintf(stderr, "internal error in server listen bind");
        exit(1);
    }
    freeaddrinfo(res);
    if (listen(server_fd,SOMAXCONN)) {
        fprintf(stderr, "internal error in server listen listen call");
        exit(1);
    }
    int session_fd=accept(server_fd,0,0);

    if (session_fd==-1) {
        fprintf(stderr, "internal error in server listen accept call");
        exit(1);
    }
    else {
        handle_session(session_fd, 0);
        close(session_fd);
        exit(0);
    }
    // now start accepting connections as they arrive
//    for (;;) {
//        int session_fd=accept(server_fd,0,0);
//        if (session_fd==-1) {
//            if (errno==EINTR) continue;
//            fprintf(stderr, "internal error in server listen accept call");
//        }
//        else {
//            handle_session(session_fd);
//            close(session_fd);
//            exit(0);
//        }
//    }

}


int main(int argc, char *argv[]) {
    int c, listen = 0, udp = 0;
    char *port = NULL;
    char *hostname = NULL;
    char usage[] = "invalid or missing options\n usage: snc [-l] [-u] [hostname] port";

    while ((c = getopt(argc, argv, "lu")) != -1) {
        switch (c) {
            case 'l':
                listen = 1;
                break;
            case 'u':
                udp = 1;
                break;
        }
    }

    int server = 0, client=0;
    port = argv[argc-1];
    hostname = argv[argc-2];
    if (listen)
        server = server_listen(hostname, port, udp);
    else {
        client = init_connect(hostname, port, udp);
    }
}

// create connection for not listening
// listen for connections for listening
// handle conetion -- for echoing


//    int port = strtol(argv[argc-1]);

//
//    }
//    if(!listen){
//        if((udp && argc !=4) || (!udp && argc !=3)){
//            fprintf(stderr, usage);
//            exit(1);
//        }
//    }
//}


//    int listen_fd, comm_fd, sock_fd;
//struct sockaddr_in servaddr;
//struct hostent *server;
//char sendline[1064];

//if (udp) {
//if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//fprintf(stderr, "internal error in udp init server socket");
//exit(1);
//}
//} else {
//if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//fprintf(stderr, "internal error in udp init server socket");
//exit(1);
//}
//}
//
//memset(&servaddr, 0, sizeof(servaddr));
//servaddr.sin_family = AF_INET;


//
//int handle_session(int sockfd){
//    char data[2048];
//    char buf[2048];
//    size_t len;
//    while(1){
//        len = read(sockfd, data, 2048);
//        data[len] = '\0';
//        fprintf(stdout,"%s", data);
//
//        memset(&buf,0, sizeof(buf));
//        ssize_t bytes = read(0,buf,2048);
//
//        if (bytes==0){
//            close(sockfd);
//            exit(0);
//        }
//
//        send(sockfd, buf, strlen(buf) +1,0);
//    }
//}