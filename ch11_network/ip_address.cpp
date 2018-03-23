//
// Created by yansheng on 18-3-23.
//

#include <cstdio>
#include <iostream>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    uint32_t ip;
    sscanf(argv[1], "%x", &ip);
    uint32_t ip_net = htonl(ip);


    char ip_string[32];
    inet_ntop(AF_INET, &ip_net, ip_string, 32);

    std::cout << ip_string << std::endl;

    return 0;
};

