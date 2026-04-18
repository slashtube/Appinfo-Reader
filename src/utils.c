#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "../include/utils.h"


void init_table(struct table* tab) {
    tab->strings = malloc(sizeof(char*) * tab->count);

    char ** strings = tab->strings;

    for(uint32_t i = 0; i < tab->count; i++) {
        *(strings + i) = (char*) malloc(sizeof(char) * LEN);
    }
}

void init_appentry(struct appentry* entry) {
    entry->texthash = malloc(sizeof(char) * HASHLEN);
    entry->binaryhash = malloc(sizeof(char) * HASHLEN);
}

void free_table(struct table* tab) {
    char ** strings = tab->strings;

    for(uint32_t i = 0; i < tab->count; i++) {
        free(*(strings + i));
    }

    free(tab->strings);
}

void free_appentry(struct appentry* entry) {
    free(entry->binaryhash);
    free(entry->texthash);
}

void print_UnixTime(uint32_t lastupdate) {
    time_t raw = (time_t) lastupdate;
    struct tm ts = *localtime(&raw);
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%d-%m-%Y", &ts);

    printf("entry.lastupdate: %s\n", buffer);
}

void print_header(struct header head) {
    printf("header.magic: %x\n", head.magic);
    printf("header.universe: %d\n", head.universe);
    printf("header.offset: %ld\n", head.offset);

}

void print_table(struct table tab) {
    printf("\ntab.count: %d\n", tab.count);
    printf("tab.strings: WIP \n");
}

void print_entry(struct appentry entry) {
    printf("\nentry.appid: %d\n", entry.appid);
    printf("entry.size: %d\n", entry.size);
    printf("entry.info: %d\n", entry.info);
    print_UnixTime(entry.lastupdate);
    printf("entry.pics: %ld\n", entry.pics);
    printf("entry.texthash: %s\n", entry.texthash);
    printf("entry.changenum: %d\n", entry.changenum);
    printf("entry.section: %d\n", entry.section);
    printf("entry.binaryhash: %s\n", entry.binaryhash);
}
