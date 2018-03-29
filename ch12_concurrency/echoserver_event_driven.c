//
// Created by yansheng on 18-3-27.
//
//
// Created by yansheng on 18-3-27.
//

#include "../csapp.h"

void echo(int connfd);



typedef struct {
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
} pool;

int byte_cnt = 0;

void init_pool(int listenfd, pool* p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    socklen_t clientfd_len;
    struct sockaddr_storage clientaddr;

    listenfd = Open_listenfd(argv[1]);

    static pool pool;

    init_pool(listenfd, &pool);

    while(1) {
        pool.ready_set = pool.read_set;
        pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &pool.ready_set)) {
            clientfd_len = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientfd_len);
            add_client(connfd, &pool);
        }
        check_clients(&pool);
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

void init_pool(int listenfd, pool* p) {
    p->maxi = -1;
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        p->clientfd[i] = -1;
    }
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}


void add_client(int connfd, pool *p) {
    p->nready--;
    int i;
    for (i = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] >=0 ) {
            continue;
        }
        p->clientfd[i] = connfd;
        Rio_readinitb(&p->clientrio[i], connfd);
        FD_SET(connfd, &p->read_set);

        if (connfd > p->maxfd) {
            p->maxfd = connfd;
        }
        if (i > p->maxfd) {
            p->maxi = i;
        }
        if (i == FD_SETSIZE) {
            app_error("add_client error: Too many clients");
        }
    }
}

void check_clients(pool *p) {
    int i = 0;
    while(i < p->maxi && p->nready > 0) {
        int connfd = p->clientfd[i];
        if (connfd < 0 || !FD_ISSET(connfd, &p->ready_set)) {
            i++;
            continue;
        }
        char buf[MAXLINE];
        ssize_t n = 0;
        if ( (n = Rio_readlineb(&p->clientrio[i], buf, MAXLINE)) != 0) {
            byte_cnt += n;
            Rio_writen(connfd, buf, n);
        } else {
            p->clientfd[i] = -1;
            FD_CLR(connfd, &p->read_set);
            Close(connfd);
        }
        i++;
        p->nready--;
    }
}