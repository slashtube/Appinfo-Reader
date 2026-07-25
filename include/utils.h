#ifndef UTILS_H
#define UTILS_H

#define LEN 256
#define HASHLEN 20

#include <stdint.h>
#include "parser.h"

void init_table(struct table* entry);
void init_appentry(struct appentry* entry);

void free_table(struct table* entry);
void free_appentry(struct appentry* entry);

void print_UnixTime(uint32_t lastupdate);
void print_header(struct header head);
void print_table(struct table tab);
void print_entry(struct appentry entry);

#endif
