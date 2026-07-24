#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include "../include/utils.h"

/*
    ------ ALLOCATION AND DEALLOCATION ------
*/

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


/*
    ------ PRINT FUNCTIONS ------

    These functions should only be used for debugging but for now they are the only way to 
    actually get the output of the program.
*/

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
}

void print_entry(struct appentry entry) {
    printf("\nentry.appid: %d\n", entry.appid);
    printf("entry.size: %d\n", entry.size);
    printf("entry.info: %d\n", entry.info);
    print_UnixTime(entry.lastupdate);
    printf("entry.pics: %ld\n", entry.pics);
    //printf("entry.texthash: %02x\n", entry.texthash);
    printf("entry.changenum: %d\n", entry.changenum);
    printf("entry.section: %d\n", entry.section);
    //printf("entry.binaryhash: %02x\n", entry.binaryhash);
}



