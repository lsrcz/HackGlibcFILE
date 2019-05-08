//
// Created by lusirui on 5/7/19.
//

#include "hack.h"
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

void print_buf(const char *buf, size_t size) {
  if (size >= 0) {
    for (size_t i = 0; i < size; ++i) {
      if (isalnum(buf[i]))
        write(STDERR_FILENO, (const char *)buf + i, 1);
      else if (((const char *)buf)[i] == '\n')
        write(STDERR_FILENO, "\\n", 2);
      else {
        write(STDERR_FILENO, "\\x", 2);
        char c = ((const char *)buf)[i];
        static const char list[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        char out[] = {list[c / 16], list[c % 16]};
        write(STDERR_FILENO, out, 2);
      }
    }
  }
}

ssize_t tracking_write(int fd, const void *buf, size_t size) {

  char buf1[std::max(size + 50, (size_t)1024)];
  int n = sprintf(buf1, "write(%d, \"", fd);
  write(STDERR_FILENO, buf1, n);
  print_buf((const char *)buf, size);
  int ret = write(fd, buf, size);
  n = sprintf(buf1, "\", %ld) = %d\n", size, ret);
  write(STDERR_FILENO, buf1, n);
  return ret;
}

ssize_t tracking_read(int fd, void *buf, size_t size) {
  ssize_t ret = read(fd, buf, size);
  char buf1[std::max(size + 50, (size_t)1024)];
  int n = sprintf(buf1, "read(%d, 0x%lx, %ld) = %d", fd, (unsigned long)buf, size,
                  ret);
  write(STDERR_FILENO, buf1, n);
  if (ret > 0) {
    write(STDERR_FILENO, " => ", 4);
    print_buf((const char *)buf, ret);
  }
  write(STDERR_FILENO, "\n", 1);
  return ret;
}

int tracking_close(int fd) {
  int ret = close(fd);
  char buf1[1024];
  int n = sprintf(buf1, "close(%d) = %d\n", fd, ret);
  write(STDERR_FILENO, buf1, n);
  return ret;
}

int tracking_open(const char *buf, int mode, ...) {
  int var = 0;
  if (mode & O_CREAT) {
    va_list ap;
    va_start(ap, mode);
    var = va_arg(ap, int);
    va_end(ap);
  }
  int ret = open(buf, mode, var);
  char buf1[strlen(buf) + 50];
  int n;
  if (mode & O_CREAT) {
    n = sprintf(buf1, "open(%s, %d, %d) = %d\n", buf, mode, var, ret);
  } else {
    n = sprintf(buf1, "open(%s, %d) = %d\n", buf, mode, ret);
  }
  write(STDERR_FILENO, buf1, n);
  return ret;
}

int main() {
  inject_write(tracking_write);
  inject_read(tracking_read);
  inject_close(tracking_close);
  inject_open(tracking_open);

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
