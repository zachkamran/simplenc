#ifndef SIMPLENC_LIBRARY_H
#define SIMPLENC_LIBRARY_H


int handle_session(int session, int client);
int init_connect(char *host,char *port,int udp);
int read_socket(int sockfd);
int server_listen(char *host, char* port, int udp);
int send_stdin(int sockfd);
void *read_socket_client(void* arg);
void *send_stdin_server(void *arg);

#endif