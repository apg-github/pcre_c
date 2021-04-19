#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *file) {
    // create an initial buffer of 10
    size_t capacity = 10;
    char *buffer = malloc(capacity * sizeof(char));
    if(!buffer) return NULL; // error

    // get lines until we have one
    size_t length = 0;
    while(fgets(buffer+length, capacity-length, file)) {
        size_t read = strlen(buffer+length);

        // if we read the whole line then there is a newline at the end
        if((buffer+length)[read-1] == '\n') {
            (buffer+length)[read-1] = '\0'; // remove the newline at the end
            break;
        }

        // if we didn't read an entire line then realloc
        capacity += 10;
        char *temp = realloc(buffer, capacity * sizeof(char));
        if(!temp) return buffer; // error - return partial string
        buffer = temp;

        length += read;
    }
    length += strlen(buffer+length);

    // if we have one return it
    if(length) return buffer;
    // otherwise return NULL
    free(buffer);
    return NULL;
}

char **read_file(FILE *file) {
    // create an initial buffer of 10
    size_t capacity = 1;
    char **array = malloc(capacity * sizeof(char*));
    if(!array) return NULL; // error

    // get one entry
    size_t length = 0;
    while((array[length] = read_line(file))) {
        if(!array[length]) break;

        if(length == capacity-1) { // have we used all the spaces?
            capacity += 1;
            char **temp = realloc(array, capacity * sizeof(char*));
            if(!temp) return array; // error - return partial string
            array = temp;
        }

        length++;
    }

    if(length) return array;

    free(array);
    return NULL;
}
    

int main() {
    FILE *file = fopen("L_2005149PL.01002201.xml.html","r");
    if(file) {
        char **array = read_file(file);
        if(array) {
            // print the file
            for(int i=0; array[i]; i++)
                printf("%s\n",array[i]);
            
            // now free the array
            for(int i=0; array[i]; i++)
                free(array[i]);
            free(array);
        }
    }
    fclose(file);
}