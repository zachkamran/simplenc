#ifndef SIMPLENC_LIBRARY_H
#define SIMPLENC_LIBRARY_H


int handle_session(int session);
int init_connect(char *host,char *port,int udp);
void *read_socket(void *arg);
int server_listen(char *host, char* port, int udp);
int send_stdin(int sockfd);

#endif