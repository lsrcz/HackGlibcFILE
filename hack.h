//
// Created by lusirui on 5/7/19.
//

#ifndef HACKGLIBCFILE_HACK_H
#define HACKGLIBCFILE_HACK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __APPLE__
#define _GNU_SOURCE
#define _IO_MTSAFE_IO
typedef struct {
    int lock;
    int cnt;
    void *owner;
} _IO_lock_t;
#endif
#include <stdio.h>
#include <sys/param.h>

#ifndef __APPLE__

typedef int64_t off64_t;

typedef void (*_IO_finish_t)(FILE *, int);

typedef int (*_IO_overflow_t)(FILE *, int);

typedef int (*_IO_underflow_t)(FILE *);

typedef int (*_IO_pbackfail_t)(FILE *, int);

typedef size_t (*_IO_xsputn_t)(FILE *FP, const void *DATA, size_t N);

typedef size_t (*_IO_xsgetn_t)(FILE *FP, void *DATA, size_t N);

typedef off64_t (*_IO_seekoff_t)(FILE *FP, off64_t OFF, int DIR, int MODE);

typedef off64_t (*_IO_seekpos_t)(FILE *, off64_t, int);

typedef FILE *(*_IO_setbuf_t)(FILE *, char *, ssize_t);

typedef int (*_IO_sync_t)(FILE *);

typedef int (*_IO_doallocate_t)(FILE *);

typedef ssize_t (*_IO_read_t)(FILE *, void *, ssize_t);

typedef ssize_t (*_IO_write_t)(FILE *, const void *, ssize_t);

typedef off64_t (*_IO_seek_t)(FILE *, off64_t, int);

typedef int (*_IO_close_t)(FILE *); /* finalize */

typedef int (*_IO_stat_t)(FILE *, void *);

typedef int (*_IO_showmanyc_t)(FILE *);

typedef void (*_IO_imbue_t)(FILE *, void *);

void inject_stat(_IO_stat_t func);

#endif

typedef int (*_unix_close_t)(int fd);

#ifndef __APPLE__

typedef ssize_t (*_unix_read_t)(int fd, void *buf, ssize_t size);

typedef off64_t (*_unix_seek_t)(int fd, off64_t offset, int whence);

typedef ssize_t (*_unix_write_t)(int fd, const void *buf, ssize_t size);

#else

typedef ssize_t (*_unix_read_t)(int fd, void *buf, size_t size);

typedef fpos_t (*_unix_seek_t)(int fd, fpos_t offset, int whence);

typedef ssize_t (*_unix_write_t)(int fd, const void *buf, size_t size);

#endif

typedef int (*_unix_open_t)(const char *buf, int oflag, ...);

typedef int (*_unix_fcntl_t)(int fd, int flag, ...);

typedef int (*_unix_dup2_t)(int fd1, int fd2);

void inject_read(_unix_read_t func);

void inject_write(_unix_write_t func);

void inject_seek(_unix_seek_t func);

void inject_close(_unix_close_t func);

void inject_open(_unix_open_t func);

void inject_fcntl(_unix_fcntl_t func);

void inject_dup2(_unix_dup2_t func);

FILE *fopen_injected(const char *filename, const char *mode);

FILE *fdopen_injected(int fd, const char *mode);

FILE *freopen_injected(const char *__restrict file, const char *__restrict mode,
                       FILE *fp);

#ifdef __cplusplus
};
#endif

#endif // HACKGLIBCFILE_HACK_H
