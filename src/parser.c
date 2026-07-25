#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "../include/parser.h"
#include "../include/utils.h"
#include "../include/reader.h"

FILE* in = NULL;
FILE* out = NULL;

/*
   Parses the header:
    - Checks the magic field
    - Reads the string table offset if field is > 40
*/

static int parse_header(struct header* head) {
    size_t ret = fread(head, sizeof(head->magic), 2, in);
    if(head->magic > VERSION_40) {
        ret += fread(&head->offset, sizeof(head->offset), 1, in);
    }

    return ret;
}

/*
   Parses the table used for string deduplication.
   Only used for versions > 40
*/

static int parse_table(struct table* tab, long offset) {
    long prev_offset = ftell(in);

    fseek(in, offset, SEEK_SET);
    size_t ret = fread(tab, sizeof(tab->count), 1, in);

    print_table(*tab);
    init_table(tab);

    for(uint32_t i = 0; i < tab->count; i++) {
        readNullString(*(tab->strings + i));
    }


    // restores previous offset
    fseek(in, prev_offset, SEEK_SET);


    return ret;
}

/*
    Parses AppEntry info before reading the binary vdf data
*/

static size_t parse_entry(struct appentry* entry, uint32_t version) {
    entry->section = 0;

    // Gets the end position of the binary vdf data
    fread(&entry->size, sizeof(uint32_t), 1, in);
    size_t end = ftell(in) + entry->size;

    fread(&entry->info, sizeof(uint32_t), 2, in);

    fread(&entry->pics, sizeof(entry->pics), 1, in);
    fread(entry->texthash, sizeof(char), 20, in);
    fread(&entry->changenum, sizeof(entry->changenum), 1, in);

    if(version < VERSION_38) {
        fread(&entry->section, sizeof(entry->section), 1, in);
    } 

    if(version >= VERSION_40) {
        fread(entry->binaryhash, sizeof(char), 20, in);
    }


    return end;
}

/*
    Main parsing function.
    Calls the parse_header, parse_table and parses all the app entries of the in.
*/

int parse(const char* inputfile, const char* outputfile) {
    in = fopen(inputfile, "rb");
    out = fopen(outputfile, "w");


    if(!in) {
        perror("Error while opening specified in");
        return -1;
    }

    // Header parsing
    struct header head;
    int ret = parse_header( &head);

    if(ret < 3) {
        perror("Error while parsing header\n");
        return -1;
    }

    print_header(head);

    // String table parsing if version is greater than 40
    struct table tab; 
    if(head.magic > VERSION_40) {
        parse_table(&tab, head.offset);

    }

    // AppEntry parsing
    struct appentry entry;

    init_appentry(&entry);
    
    while(1) {
        // Appid check
        fread(&entry.appid, sizeof(uint32_t), 1, in);
        if(entry.appid == 0) {
            break;
        }

        size_t end = parse_entry(&entry, head.magic);

        // Debug only
        //print_entry(entry);

        // Reads appentry data
        readVDFBlob(end, tab);

    }

    free_appentry(&entry);
    free_table(&tab);
    fclose(out);
    fclose(in);

    return 0;
}