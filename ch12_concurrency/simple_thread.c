//
// Created by yansheng on 18-3-29.
//

#include "../csapp.h"

void *hello(void *vargp) {
    printf("hello, world\n");
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    Pthread_create(&tid, NULL, hello, NULL);
    Pthread_join(tid, NULL);

    return 0;
}
