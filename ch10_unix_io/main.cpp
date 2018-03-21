//
// Created by yansheng on 18-3-21.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>



int main() {
    char buf[1024];
    ssize_t n_read = read(STDIN_FILENO, buf, 1024);
    std::cout << n_read << std::endl;
    return 0;
}
