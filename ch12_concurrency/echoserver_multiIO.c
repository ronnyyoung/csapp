//
// Created by yansheng on 18-3-27.
//

#include "../csapp.h"

void echo(int connfd);


int main(int argc, char *argv[]) {
    int listenfd, connfd;
    socklen_t clientfd_len;
    struct sockaddr_storage clientaddr;

    listenfd = Open_listenfd(argv[1]);

    fd_set read_set, ready_set;
    FD_ZERO(&read_set);                 // 清空 read set
    FD_SET(STDIN_FILENO, &read_set);    // 将标准输入加入read set中
    FD_SET(listenfd, &read_set);        // 将监听描述符加到read set中


    while(1) {
        ready_set = read_set;
        printf("start select\n");
        Select(listenfd + 1, &ready_set, NULL, NULL, NULL);
        printf("select ready\n");
        if (FD_ISSET(STDIN_FILENO, &ready_set)) {
            printf("enter stdin\n");
            // 处理server服务器上的命令行读取
            char buf[MAXLINE];
            if (!Fgets(buf, MAXLINE, stdin))
                exit(0);
            printf("%s", buf);
        }
        if (FD_ISSET(listenfd, &read_set)) {
            printf("enter listenfd\n");
            clientfd_len = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientfd_len);
            echo(connfd);
            Close(connfd);
            printf("Connection closed.\n");
        }
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