#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <sys/types.h>
#include "./parser.h"

#define TYPE_MAP 0x00
#define TYPE_STRING 0x01
#define TYPE_INT 0x02
#define MAP_END 0x08

size_t get_name(FILE* file, struct table tab); 
size_t read_int(FILE* file);
size_t read_string(FILE* file);
void readNullString(FILE* file, char* string);
size_t read_next(FILE* file, struct table tab);
void readVDFBlob(FILE* file, size_t end, struct table tab);

#endif