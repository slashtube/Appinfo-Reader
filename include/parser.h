#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#define VERSION_36 0x07564424
#define VERSION_38 0x07564426
#define VERSION_40 0x07564428

extern FILE* in;
extern FILE* out;

struct header {
    uint32_t magic;
    uint32_t universe;
    int64_t offset;
};

struct table {
    uint32_t count;
    char** strings;
};

struct appentry {
    uint32_t appid; // if 0, EOF
    uint32_t size; // Size of app entry
    uint32_t info;
    uint32_t lastupdate;
    uint64_t pics;
    char* texthash;
    uint32_t changenum;
    uint8_t section;
    char* binaryhash;
};

int parse(char* filename);


#endif
