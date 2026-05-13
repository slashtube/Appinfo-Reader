#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "../include/parser.h"
#include "../include/utils.h"


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
    
    while(true) {
        // Appid check
        fread(&entry.appid, sizeof(uint32_t), 1, file);
        if(entry.appid == 0) {
            break;
        }

        size_t end = parse_entry(file, &entry, head.magic);
        print_entry(entry);
        readVDFBlob(file, end, tab);

    }


    free_appentry(&entry);
    free_table(&tab);
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

    print_table(*tab);
    init_table(tab);

    for(uint32_t i = 0; i < tab->count; i++) {
        readNullString(file, *(tab->strings + i));
    }


    // restores previous offset
    fseek(file, prev_offset, SEEK_SET);


    return ret;
}

size_t parse_entry(FILE* file, struct appentry* entry, uint32_t version) {
    entry->section = 0;


    // Gets the end position of the cursor
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

void readNullString(FILE* file, char* string) {
    int i = 0;
    char c;
    while((c = fgetc(file)) != '\0') {
        *(string + i++) = c;
    }

    *(string + i) = '\0';

}

size_t read_next(FILE* file, struct table tab) {
    char type;
    size_t read = 0;

    // Gets type
    read += fread(&type, sizeof(char), 1, file);

    while(type != MAP_END) {
        // String deduplication
        read += get_name(file, tab);

        // Gets and writes value based on type
        switch(type) {
            case TYPE_MAP: 
                printf("{\n ");
                read += read_next(file, tab);
                return read;
            case TYPE_STRING: 
                read += read_string(file);
                break;
            case TYPE_INT: 
                read += read_int(file);
                break;
            default:
                printf("Type: %x not supported", type);
        }

        read += fread(&type, sizeof(char), 1, file);
    }

    printf("}\n");

    return read;
}

void readVDFBlob(FILE* file, size_t end, struct table tab) {
    const size_t bytes_to_read = end - ftell(file);
    size_t read = 0;

    while(read < bytes_to_read) {
        read += read_next(file, tab);
    }
}

