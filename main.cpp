//
// Created by lusirui on 5/7/19.
//

#include "hack.h"
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

void print_buf(const char *buf, size_t size) {
  struct ctrl {
    char c;
    char buf[3];
  } ctrls[] = {{'\t', "\\t"},
               {'\f', "\\f"},
               {'\v', "\\v"},
               {'\n', "\\n"},
               {'\r', "\\r"}};
  if (size >= 0) {
    for (size_t i = 0; i < size; ++i) {
      if (isprint(buf[i])) {
        write(STDERR_FILENO, (const char *)buf + i, 1);
      } else {
        ctrl* b = std::begin(ctrls);
        for (; b != std::end(ctrls); ++b) {
          if (b->c == buf[i]) {
            write(STDERR_FILENO, b->buf, 2);
            break;
          }
        }
        if (b == std::end(ctrls)) {
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
}
#ifndef __APPLE__
ssize_t tracking_write(int fd, const void *buf, ssize_t size)
#else
ssize_t tracking_write(int fd, const void *buf, size_t size)
#endif
{

  char buf1[std::max((size_t)size + 50, (size_t)1024)];
  int n = sprintf(buf1, "write(%d, \"", fd);
  write(STDERR_FILENO, buf1, n);
  print_buf((const char *)buf, size);
  int ret = write(fd, buf, size);
  n = sprintf(buf1, "\", %ld) = %d\n", size, ret);
  write(STDERR_FILENO, buf1, n);
  return ret;
}

#ifndef __APPLE__
ssize_t tracking_read(int fd, void *buf, ssize_t size)
#else
ssize_t tracking_read(int fd, void *buf, size_t size)
#endif
{
  ssize_t ret = read(fd, buf, size);
  char buf1[std::max((size_t)size + 50, (size_t)1024)];
  int n = sprintf(buf1, "read(%d, 0x%lx, %ld) = %d", fd, (unsigned long)buf,
                  size, ret);
  write(STDERR_FILENO, buf1, n);
  if (ret > 0) {
    write(STDERR_FILENO, " => \"", 5);
    print_buf((const char *)buf, ret);
    write(STDERR_FILENO, "\"", 1);
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

int tracking_dup2(int fd1, int fd2) {
  int ret = dup2(fd1, fd2);
  char buf1[50];
  int n = sprintf(buf1, "dup(%d, %d) = %d\n", fd1, fd2, ret);
  write(STDERR_FILENO, buf1, n);
  return ret;
}

int main() {
    int aaa = stdout->_flags;
  inject_write(tracking_write);
  inject_read(tracking_read);
  inject_close(tracking_close);
  inject_open(tracking_open);
  inject_dup2(tracking_dup2);

  int fd = open("a", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  FILE *file = fdopen_injected(fd, "w");
  fprintf(file, "s ");
  fflush(file);
  fprintf(file, "s ");
  fclose(file);
  FILE *file1 = fopen_injected("a", "r");
  char buf[100];
  fscanf(file1, "%s", buf);
  fscanf(file1, "%s", buf);
  fclose(file1);

  fprintf(stdout, "a");
  fputc('\n', stdout);
  fprintf(stdout, "b");
  fprintf(stderr, "a");
  fprintf(stderr, "b");

  int aa = stdout->_flags;
  freopen_injected("a", "w", stdout);
    int b = stdout->_flags;
  printf("123456");
  fflush(stdout);
    int c = stdout->_flags;

    char buffff[1000];
    sprintf(buffff, "%x, %x, %x, %x", aaa, aa, b, c);

  freopen_injected("a", "r", stdin);
  int a;
  scanf("%d", &a);
  fprintf(stderr, "%d\n", a);

  return 0;
}

