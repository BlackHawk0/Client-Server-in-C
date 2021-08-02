#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "remote.h"

#define BUFSIZE 1024



int connect_remote(char *address,int port){
    char *serv_name = address;
    int sockfd;
    int flags;
    int serv_port = port;
    struct sockaddr_in serv_addr;
    
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, serv_name, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(serv_port);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("Could not create socket\n");
        return -1;
    }
    flags = fcntl(sockfd, F_GETFL, 0);
    if(fcntl(sockfd,F_SETFL,flags|O_NONBLOCK)<0){
        perror("Could not set socket into non-blocking\n");
    }
    if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        perror("Could not connect\n");
        return -1;
    }

    return sockfd;
}

int remote_echo(char *server,char *text){
    int port = 3494;
    int fd = connect_remote(server, port);
    if (fd<0) return -1;
    
    send(fd, text, strlen(text),0);
    char *buf[BUFSIZE];
    int count;
    while ((count=recv(fd,buf,BUFSIZE,0))>0){
        write(STDOUT_FILENO, buf, BUFSIZE);
    }
    close(fd);
    return 0;
}

int remote_code(char *server,char *name_id){
    int port = 3494;
    int fd = connect_remote(server, port);
    if (fd<0) return -1;

    send(fd, name_id, strlen(name_id), 0);
    char *buf[BUFSIZE];
    int count;
    while ((count=recv(fd,buf,BUFSIZE,0))>0){
        write(STDOUT_FILENO, buf, BUFSIZE);
    }
    close(fd);
    return 0;
}

int remote_msg(char *server){
    int port = 3496;
    int fd = connect_remote(server, port);
    if (fd<0) return -1;

    char *buf[BUFSIZE];
    int count;
    while ((count=recv(fd,buf,BUFSIZE,0))>0){
        write(STDOUT_FILENO, buf, BUFSIZE);
    }
    close(fd);
    return 0;
}
