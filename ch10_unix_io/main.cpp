//
// Created by yansheng on 18-3-21.
//

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>



int main() {
    int fd = open("/home/yansheng/Pictures/yys.jpg", O_RDONLY, 0);
    off_t file_end = lseek(fd, 0, SEEK_END);
    off_t file_start = lseek(fd, 0, SEEK_SET);
    size_t file_size = file_end - file_start;

    char *buffer = new char[file_size];
    read(fd, buffer, file_size);

    mode_t file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    mode_t mask = S_IWGRP | S_IWOTH;
    umask(mask);
    int pic_fd = open("tmp.jpg", O_CREAT|O_RDWR, file_mode);
    write(pic_fd, buffer, file_size);
    close(fd);
    close(pic_fd);

    delete[] buffer;

    return 0;
}
