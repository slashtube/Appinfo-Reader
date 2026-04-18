#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/parser.h"
#include "../include/utils.h"


int parse(char* filename) {
    FILE* file = fopen(filename, "r");
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

        print_table(tab);
    }

    // AppEntry parsing
    struct appentry entry;

    init_appentry(&entry);
    
    int i = 0;

    while(i < 1) {
        parse_entry(file, &entry, head.magic);
        print_entry(entry);

        // TODO: Read binary VDF data
        i++;
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

    init_table(tab);

    // TODO: Read null terminated strings

    // restores previous offset
    fseek(file, prev_offset, SEEK_SET);


    return ret;
}

void parse_entry(FILE* file, struct appentry* entry, uint32_t version) {
    entry->section = 0;

    // TODO: check appid
    fread(entry, sizeof(uint32_t), 4, file);

    fread(&entry->pics, sizeof(entry->pics), 1, file);
    fread(entry->texthash, sizeof(char), 20, file);
    fread(&entry->changenum, sizeof(entry->changenum), 1, file);

    if(version < VERSION_38) {
        fread(&entry->section, sizeof(entry->section), 1, file);
    } 

    if(version >= VERSION_40) {
        fread(entry->binaryhash, sizeof(char), 20, file);
    }

}


