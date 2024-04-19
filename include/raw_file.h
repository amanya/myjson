#ifndef RAW_FILE_H
#define RAW_FILE_H

typedef struct RAW_FILE {
    char *file_name;
    long long file_size;
    char *buf;
} RawFile;

RawFile *raw_file_create(char *file_name);
void raw_file_destroy(RawFile *raw_file);
int raw_file_read(RawFile *raw_file);

#endif
