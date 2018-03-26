//
// Created by yansheng on 18-3-26.
//

#include "../csapp.h"

int main(int argc, char *argv[]) {
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    char *buf, *p;

    int n1 = 0, n2 = 0;
    if ((buf = getenv("QUERY_STRING")) != NULL) {
        p = strchr(buf, '&');
        *p = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, p + 1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    sprintf(content, "%d + %d = %d\r\n", n1, n2, n1 + n2);

    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);

    exit(0);
}
