#include "utils.h"

void build_string(char *buffer, const char *command, const char **strings, int n_strings) {
    size_t total_length = strlen(command) + 1;  // Include space for null terminator

    for (int i = 0; i < n_strings; ++i) {
        total_length += strlen(strings[i]) + 1;  // Include space for null terminator
    }

    total_length += (size_t) n_strings;  // Count spaces between strings plus the \n 

    strcpy(buffer, command);

    for (int i = 0; i < n_strings; ++i) {
        strcat(buffer, " ");
        strcat(buffer, strings[i]);
    }

    strcat(buffer, "\n");
}

char *get_file_info(const char *fname) {
    FILE *file = fopen(fname, "rb");
    
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file data
    char *fdata = (char *)malloc((size_t)fsize);
    if (fdata == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read file data
    size_t bytesRead = fread(fdata, 1, (size_t)fsize, file);
    if (bytesRead != fsize) {
        perror("Error reading file");
        free(fdata);
        fclose(file);
        return NULL;
    }

    // Close the file
    fclose(file);

    // Print file size and data
    /* printf("File Size: %ld bytes\n", fsize);
    printf("File Data: %.*s\n", (int)fsize, fdata); */

    char fsize_str[20];// Suficientemente grande para armazenar a representação de um long
    sprintf(fsize_str, "%ld", fsize);

    // Calcular o tamanho necessário para a string resultante
    size_t result_size = strlen(fname) + strlen(fsize_str) + strlen(fdata) + 4;  // 4 para os espaços e a quebra de linha

    // Alocar memória para a string resultante
    char *result = (char *)malloc(result_size);
    if (result == NULL) {
        perror("Error allocating memory");
        free(fdata);
        return NULL;
    }

    // Construir a string resultante
    sprintf(result, "%s %s %s\n", fname, fsize_str, fdata);

    free(fdata);
    return result;
}
