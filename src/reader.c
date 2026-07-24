#include "../include/reader.h"
#include <stdint.h>
#include <string.h>

/*
    Defines functions used for correctly reading the binary KeyValues format of Valve.

    The binary VDF is just a repetition of a type byte, a string also called name and some values based on the type byte.

    From my experience, the appinfo file only uses these type of values: 
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

size_t get_name(FILE* file, struct table tab) {
    size_t read = 0;
    uint32_t pos;

    fread(&pos, sizeof(uint32_t), 1, file);
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

size_t read_int(FILE* file) {
    uint32_t value = 0;
    size_t read = 0;

    fread(&value, sizeof(uint32_t), 1, file);
    read = sizeof(uint32_t);

    printf("%d,\n", value);

    return read;
}


/*
    Used for reading null terminated string values
*/

size_t read_string(FILE* file) {
    size_t read = 0;
    char string[1024];

    readNullString(file, string);
    read += strlen(string) + 1;
    printf("'%s',\n", string);

    return read;
}


/*
    Aux function for read_string
*/

void readNullString(FILE* file, char* string) {
    int i = 0;
    char c;
    while((c = fgetc(file)) != '\0') {
        *(string + i++) = c;
    }

    *(string + i) = '\0';

}

/*
    Core reading function. Recursively reads for bytes until in reaches the MAP_END.
*/

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

/*
    Entrypoint for reading the binary vdf data.
    This function also makes sure that the read_next function only reads a specified amount of bytes.
*/

void readVDFBlob(FILE* file, size_t end, struct table tab) {
    const size_t bytes_to_read = end - ftell(file);
    size_t read = 0;

    while(read < bytes_to_read) {
        read += read_next(file, tab);
    }
}