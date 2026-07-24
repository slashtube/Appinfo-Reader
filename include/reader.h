#ifndef READER_H
#define READER_H

#include <stdio.h>
#include <sys/types.h>
#include "./parser.h"

#define TYPE_MAP 0x00
#define TYPE_STRING 0x01
#define TYPE_INT 0x02
#define MAP_END 0x08

void readNullString(FILE* file, char* string);
void readVDFBlob(FILE* file, size_t end, struct table tab);

#endif