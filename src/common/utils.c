#include "utils.h"

char *get_file_info(const char *fname, long *fsize) {
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    *fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Allocate memory for file data */
    char *fdata = (char *)malloc((size_t)*fsize);
    if (fdata == NULL) {
        printf("Error allocating memory\n");
        fclose(file);
        return NULL;
    }

    /* Read file data */
    size_t bytesRead = fread(fdata, 1, (size_t)*fsize, file);
    if (bytesRead != *fsize) {
        printf("Error reading file\n");
        free(fdata);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return fdata;
}

int is_numeric(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i])) return 0;
    }
    return 1;    
}

int is_alphanumeric(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i]) && !isalpha(buffer[i])) return 0;
    }
    return 1;
}
