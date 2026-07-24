#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "../include/parser.h"
#include "../include/utils.h"
#include "../include/reader.h"

/*
    Main parsing function.
    Calls the parse_header, parse_table and parses all the app entries of the file.
*/

int parse(char* filename) {
    FILE* file = fopen(filename, "rb");
    if(!file) {
        perror("Error while opening specified file");
        return -1;
    }

    // Header parsing
    struct header head;
    int ret = parse_header(file, &head);

    if(ret < 3) {
        perror("Error while parsing header\n");
        return -1;
    }

    print_header(head);

    // String table parsing if version is greater than 40
    struct table tab; 
    if(head.magic > VERSION_40) {
        parse_table(file, &tab, head.offset);

    }

    // AppEntry parsing
    struct appentry entry;

    init_appentry(&entry);
    
    while(1) {
        // Appid check
        fread(&entry.appid, sizeof(uint32_t), 1, file);
        if(entry.appid == 0) {
            break;
        }

        size_t end = parse_entry(file, &entry, head.magic);
        print_entry(entry);

        // Reads appentry data
        readVDFBlob(file, end, tab);

    }

    free_appentry(&entry);
    free_table(&tab);
    fclose(file);

    return 0;
}

/*
   Parses the header:
    - Checks the magic field
    - Reads the string table offset if field is > 40
*/

int parse_header(FILE* file, struct header* head) {
    size_t ret = fread(head, sizeof(head->magic), 2, file);
    if(head->magic > VERSION_40) {
        ret += fread(&head->offset, sizeof(head->offset), 1, file);
    }

    return ret;
}

/*
   Parses the table used for string deduplication.
   Only used for versions > 40
*/

int parse_table(FILE* file, struct table* tab, long offset) {
    long prev_offset = ftell(file);

    fseek(file, offset, SEEK_SET);
    size_t ret = fread(tab, sizeof(tab->count), 1, file);

    print_table(*tab);
    init_table(tab);

    for(uint32_t i = 0; i < tab->count; i++) {
        readNullString(file, *(tab->strings + i));
    }


    // restores previous offset
    fseek(file, prev_offset, SEEK_SET);


    return ret;
}

/*
    Parses AppEntry info before reading the binary vdf data
*/

size_t parse_entry(FILE* file, struct appentry* entry, uint32_t version) {
    entry->section = 0;

    // Gets the end position of the binary vdf data
    fread(&entry->size, sizeof(uint32_t), 1, file);
    size_t end = ftell(file) + entry->size;

    fread(&entry->info, sizeof(uint32_t), 2, file);

    fread(&entry->pics, sizeof(entry->pics), 1, file);
    fread(entry->texthash, sizeof(char), 20, file);
    fread(&entry->changenum, sizeof(entry->changenum), 1, file);

    if(version < VERSION_38) {
        fread(&entry->section, sizeof(entry->section), 1, file);
    } 

    if(version >= VERSION_40) {
        fread(entry->binaryhash, sizeof(char), 20, file);
    }


    return end;
}



