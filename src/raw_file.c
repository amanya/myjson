#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../include/raw_file.h"

RawFile *raw_file_create(char *file_name) {
    RawFile *raw_file = (RawFile *)malloc(sizeof(RawFile));
    raw_file->file_name = file_name;
    raw_file->file_size = 0;
    raw_file->buf = NULL;
    return raw_file;
}

void raw_file_destroy(RawFile *raw_file) {
    if (raw_file->buf != NULL) {
        free(raw_file->buf);
    }
    free(raw_file);
}

int raw_file_read(RawFile *raw_file) {
    int fd = open(raw_file->file_name, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    struct stat file_stat = {0};
    if (fstat(fd, &file_stat) < 0) {
        perror("fstat");
        close(fd);
        return -1;
    }

    raw_file->file_size = file_stat.st_size;

    raw_file->buf = (char *)malloc(raw_file->file_size + 1);
    if (raw_file->buf == NULL) {
        perror("malloc");
        close(fd);
        return -1;
    }
    memset(raw_file->buf, 0, raw_file->file_size + 1);

    ssize_t bytes_read = read(fd, raw_file->buf, raw_file->file_size);

    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return -1;
    } else if (bytes_read < raw_file->file_size) {
        fprintf(stderr, "Read less bytes than expected.\n");
        close(fd);
        return -1;
    }

    close(fd);

    return 0;
}
