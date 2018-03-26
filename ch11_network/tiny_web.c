//
// Created by yansheng on 18-3-26.
//

#include "../csapp.h"

void doit(int sock_fd);
void error_status(int sock_fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rio);

int parse_url(char *uri, char *filename, char *cgi_args);

void serve_static(int fd, char *filename, int file_size);

void serve_dynamic(int fd, char *filename, char *cgi_args);

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    char *server_port = argv[1];
    int listen_sockfd = Open_listenfd(server_port);

    struct sockaddr client_sock_addr;
    socklen_t client_sock_len;

    char client_host_name[MAXLINE];
    char client_port[MAXLINE];


    while(1) {
        client_sock_len = sizeof(client_sock_addr);
        int connect_sockfd = Accept(listen_sockfd, (SA *)&client_sock_addr, &client_sock_len);
        Getnameinfo((SA *)&client_sock_addr, client_sock_len, client_host_name, MAXLINE, client_port, MAXLINE, 0);
        fprintf(stdout, "Accepted connection from (%s, %s)\n", client_host_name, client_port);
        doit(connect_sockfd);
        Close(connect_sockfd);
    }
}


void doit(int sock_fd) {
    rio_t rio;
    char buf[MAXLINE];

    Rio_readinitb(&rio, sock_fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    fprintf(stdout, "request header: %s\n", buf);

    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    sscanf(buf, "%s %s %s", method, uri, version);


    if (strcasecmp(method, "GET") != 0) {
        error_status(sock_fd, method, "501", "Note implemented", "Tiny does not implement this method");
        return;
    }

    // 读取请求报头，直至遇到`\n\r`
    read_requesthdrs(&rio);

    char filename[MAXLINE], cgi_arg[MAXLINE];
    int is_static = parse_url(uri, filename, cgi_arg);

    struct stat sbuf;
    if(stat(filename, &sbuf) < 0) {
        error_status(sock_fd, filename, "404", "Not found", "Tiny couldn't find this file.");
        return;
    }

    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            error_status(sock_fd, filename, "403", "Forbidden", "Tiny couldn't read the file.");
            return;
        }
        serve_static(sock_fd, filename, sbuf.st_size);
    } else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            error_status(sock_fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program.");
            return;
        }
        serve_dynamic(sock_fd, filename, cgi_arg);
    }




}

void error_status(int sock_fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char reponse_msg[MAXLINE];

    char body[MAXBUF];
    sprintf(body, "<html><title>Tiny Error</title><body>");
    sprintf(body, "%s<p>%s</p>", body, longmsg);
    sprintf(body, "%s</body></html>", body);

    sprintf(reponse_msg, "HTTP/1.1 %s %s\r\n", errnum, shortmsg);
    rio_writen(sock_fd, reponse_msg, strlen(reponse_msg));
    sprintf(reponse_msg, "Content-Type: text/html\r\n");
    rio_writen(sock_fd, reponse_msg, strlen(reponse_msg));
    sprintf(reponse_msg, "Content-Length: %d\r\n\r\n", (int)strlen(body));
    rio_writen(sock_fd, reponse_msg, strlen(reponse_msg));

    rio_writen(sock_fd, body, strlen(body));
}

void read_requesthdrs(rio_t *rio) {
    char buf[MAXLINE];
    while(1) {
        Rio_readlineb(rio, buf, MAXLINE);
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
        fprintf(stdout, "%s", buf);
    }
}


int parse_url(char *uri, char *filename, char *cgi_args) {
    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgi_args, "");

        sprintf(filename, ".%s", uri);

        if(uri[strlen(uri) - 1] == '/') {
            sprintf(filename, "%s%s", filename, "index.html");
        }

        return 1;
    } else {
        char *ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgi_args, ptr + 1);
            *ptr = '\0';
        } else {
            strcpy(cgi_args, "");
        }
        sprintf(filename, ".%s", uri);
        return 0;
    }
}


void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html")) {
        strcpy(filetype, "text/html");
    } else if (strstr(filename, ".gif")) {
        strcpy(filetype, "image/gif");
    } else if (strstr(filename, ".png")) {
        strcpy(filetype, "image/png");
    } else if (strstr(filename, ".jpg")) {
        strcpy(filetype, "image/jpeg");
    } else {
        strcpy(filetype, "text/plain");
    }
}

void serve_static(int fd, char *filename, int file_size) {

    char filetype[MAXLINE];
    get_filetype(filename, filetype);

    char reponse[MAXBUF];
    sprintf(reponse, "HTTP/1.0, 200 OK\r\n");
    sprintf(reponse, "%sServer: Tiny Web Server\r\n", reponse);
    sprintf(reponse, "%sConnection: close\r\n", reponse);
    sprintf(reponse, "%sContent-length: %d\r\n", reponse, file_size);
    sprintf(reponse, "%sContent-type: %s\r\n\r\n", reponse, filetype);

    printf("Response headers: \n%s", reponse);

    Rio_writen(fd, reponse, strlen(reponse));



    int srcfd = Open(filename, O_RDONLY, 0);
    char* srcp = Mmap(0, file_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
    close(srcfd);
    Rio_writen(fd, srcp, file_size);
    Munmap(srcp, file_size);
}


void serve_dynamic(int fd, char *filename, char *cgi_args) {
    char reponse[MAXBUF];
    sprintf(reponse, "HTTP/1.0, 200 OK\r\n");
    sprintf(reponse, "%sServer: Tiny Web Server\r\n", reponse);
    Rio_writen(fd, reponse, strlen(reponse));

    if (Fork() == 0) {
        setenv("QUERY_STRING", cgi_args, 1);
        Dup2(fd, STDOUT_FILENO);
        char *emptylist[] = {NULL};
        Execve(filename, emptylist, environ);
    }

    Wait(NULL);
}

