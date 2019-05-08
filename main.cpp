//
// Created by lusirui on 5/7/19.
//

#include "hack.h"
#include <fcntl.h>
#include <unistd.h>

ssize_t tracking_write(FILE *file, const void *buf, ssize_t size) {
    write(STDOUT_FILENO, "--- Writing: ---\n", 17);
    write(STDOUT_FILENO, buf, size);
    write(STDOUT_FILENO, "\n", 1);
    return original_write(file, buf, size);
}

ssize_t tracking_read(FILE *file, void *buf, ssize_t size) {
    ssize_t ret = original_read(file, buf, size);
    if (ret < 0) {
        write(STDOUT_FILENO, "--- Reading Failed ---\n", 23);
    } else {
        write(STDOUT_FILENO, "--- Reading: ---\n", 17);
        write(STDOUT_FILENO, buf, ret);
        write(STDOUT_FILENO, "\n", 1);
    }
    return ret;
}

int main() {
    inject_write(tracking_write);
    inject_read(tracking_read);
    int fd = open("a", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    FILE *file = fdopen_injected(fd, "w");
    fprintf(file, "s");
    fflush(file);
    fprintf(file, "s");
    fclose(file);
    FILE *file1 = fopen_injected("a", "r");
    char buf[100];
    fscanf(file1, "%s", buf);
    fscanf(file1, "%s", buf);
    fclose(file1);
    return 0;
}


