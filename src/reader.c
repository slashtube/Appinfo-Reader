#include "../include/reader.h"
#include <stdint.h>
#include <string.h>

/*
    Defines functions used for correctly reading the binary KeyValues format of Valve.

    The binary VDF is just a repetition of a type byte, a string also called name and some values based on the type byte.

    From my experience, the appinfo in only uses these type of values: 
        - TYPE_MAP: marks the beginning of a new key/map entry.
        - TYPE_STRING: data to read is a null terminated string
        - TYPE_INT: data to read is a 4 byte integer
        - MAP_END: marks the end of key/map entry.

    Every Binary VDF data ends with a MAP_END and the next byte is the new entry.

    https://developer.valvesoftware.com/wiki/Binary_VDF
*/

/*
    Gets the name used to identify a map entry and deduplicates it using the table
*/

static size_t get_name(struct table tab) {
    size_t read = 0;
    uint32_t pos;

    fread(&pos, sizeof(uint32_t), 1, in);
    read += sizeof(uint32_t);

    char* string = *(tab.strings + pos);
    if(string != NULL && pos != 0) {
        printf("\t%s: ", string);
    }

    return read;
}


/*
    Used for reading int values
*/

static size_t read_int() {
    uint32_t value = 0;
    size_t read = 0;

    fread(&value, sizeof(uint32_t), 1, in);
    read = sizeof(uint32_t);

    printf("%d,\n", value);

    return read;
}

/*
    Used for reading null terminated string values
*/
static size_t read_string() {
    size_t read = 0;
    char string[1024];

    readNullString(string);
    read += strlen(string) + 1;
    printf("'%s',\n", string);

    return read;
}

/*
    Core reading function. Recursively reads for bytes until in reaches the MAP_END.
*/

static size_t read_next(struct table tab) {
    char type;
    size_t read = 0;

    // Gets type
    read += fread(&type, sizeof(char), 1, in);

    while(type != MAP_END) {
        // String deduplication
        read += get_name(tab);

        // Gets and writes value based on type
        switch(type) {
            case TYPE_MAP: 
                printf("{\n ");
                read += read_next( tab);
                return read;
            case TYPE_STRING: 
                read += read_string();
                break;
            case TYPE_INT: 
                read += read_int();
                break;
            default:
                printf("Type: %x not supported", type);
        }

        read += fread(&type, sizeof(char), 1, in);
    }

    printf("}\n");

    return read;
}

/*
    Aux function for read_string
*/

void readNullString(char* string) {
    int i = 0;
    char c;
    while((c = fgetc(in)) != '\0') {
        *(string + i++) = c;
    }

    *(string + i) = '\0';

}

/*
    Entrypoint for reading the binary vdf data.
    This function also makes sure that the read_next function only reads the required number of bytes.
*/

void readVDFBlob(size_t end, struct table tab) {
    const size_t bytes_to_read = end - ftell(in);
    size_t read = 0;

    while(read < bytes_to_read) {
        read += read_next( tab);
    }
}