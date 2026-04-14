#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/parser.h"


int parse(char* filename) {
    FILE* file = fopen(filename, "r");
    if(!file) {
        perror("Error while opening specified file");
        return -1;
    }

    // Header parsing
    struct header head;
    int ret = parse_header(file, &head);

    printf("header.magic: %x\n", head.magic);
    printf("header.universe: %d\n", head.universe);
    printf("header.offset: %ld\n", head.offset);

    if(ret < 3) {
        perror("Error while parsing header\n");
        return -1;
    }

    // String table parsing if version is greater than 40
    if(head.magic > VERSION_40) {
        struct table tab;
        parse_table(file, &tab, head.offset);

        printf("\ntab.count: %d\n", tab.count);
        printf("tab.strings: WIP \n");
    }

    // Appentry parsing
    struct appentry entry;

    entry.texthash = malloc(sizeof(char) * 20);
    entry.binaryhash = malloc(sizeof(char) * 20);

    parse_entry(file, &entry, head.magic);

    printf("\nentry.appid: %d\n", entry.appid);
    printf("entry.size: %d\n", entry.size);
    printf("entry.info: %d\n", entry.info);
    printf("entry.lastupdate: %d\n", entry.lastupdate);
    printf("entry.pics: %ld\n", entry.pics);
    printf("entry.texthash: %s\n", entry.texthash);
    printf("entry.changenum: %d\n", entry.changenum);
    printf("entry.section: %d\n", entry.section);
    printf("entry.binaryhash: %s\n", entry.binaryhash);

    free(entry.binaryhash);
    free(entry.texthash);
    fclose(file);

    return 0;
}

/*
   Parses the header, if successful returns the number of fields correctly read (3)
*/

int parse_header(FILE* file, struct header* head) {
    size_t ret = fread(head, sizeof(head->magic), 2, file);
    if(head->magic > VERSION_40) {
        ret += fread(&head->offset, sizeof(head->offset), 1, file);
    }

    return ret;
}

int parse_table(FILE* file, struct table* tab, long offset) {
    long prev_offset = ftell(file);

    fseek(file, offset, SEEK_SET);
    size_t ret = fread(tab, sizeof(tab->count), 1, file);

    // restores previous offset
    fseek(file, prev_offset, SEEK_SET);


    return ret;
}

int parse_entry(FILE* file, struct appentry* entry, uint32_t version) {
    entry->section = 0;
    size_t ret = fread(entry, sizeof(entry->appid), 4, file);
    ret += fread(entry, sizeof(entry->pics), 1, file);
    ret += fread(entry, sizeof(char) * 20, 1, file);
    ret += fread(entry, sizeof(entry->changenum), 1, file);

    if(version < VERSION_38) {
        ret += fread(entry, sizeof(entry->section), 1, file);
    }

    if(version >= VERSION_40) {
        ret += fread(entry, sizeof(char) * 20, 1, file);
    }

    return ret;
}
