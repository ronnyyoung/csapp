//
// Created by yansheng on 18-3-27.
//

// Echo 服务端的并发模式

#include "../csapp.h"

void echo(int connfd);

void sigchld_handler(int sig) {
    while(waitpid(-1, 0, WNOHANG) > 0)
        ;
    return;
}

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    socklen_t clientfd_len;
    struct sockaddr_storage clientaddr;

    // 注册SIGCHLD信号的处理函数
    Signal(SIGCHLD, sigchld_handler);

    listenfd = Open_listenfd(argv[1]);
    while(1) {
        clientfd_len = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientfd_len);
        if (Fork() == 0) {
            Close(listenfd);
            echo(connfd);
            Close(connfd);
            exit(0);
        }
        Close(connfd);
    }
}



void  echo(int connfd) {
    size_t n;
    char buf[MAXLINE];

    rio_t rio;
    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", (int)n);
        Rio_writen(connfd, buf, n);
    }
}