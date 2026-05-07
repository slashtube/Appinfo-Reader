#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

#define VERSION_36 0x07564424
#define VERSION_38 0x07564426
#define VERSION_40 0x07564428

#define TYPE_MAP 0x00
#define TYPE_STRING 0x01
#define TYPE_INT 0x02
#define MAP_END 0x08

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
    char* texthash; // hash da 20 byte per appinfo VDF
    uint32_t changenum;
    uint8_t section;
    char* binaryhash; // hash da 20 byte per VDF data
};

int parse(char* filename);
int parse_header(FILE* file, struct header* head);
int parse_table(FILE* file, struct table* tab, long offset);
size_t parse_entry(FILE* file, struct appentry* entry, uint32_t version);
void readNullString(FILE* file, char* string);
size_t read_next(FILE* file, struct table tab);
void readVDFBlob(FILE* file, size_t end, struct table tab);

#endif
