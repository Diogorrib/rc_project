#include "utils.h"

char *build_string(const char *command, const char **strings, int n_strings) {
    size_t total_length = strlen(command);
    for (int i = 0; i < n_strings; ++i) {
        total_length += strlen(strings[i]);
    }
    total_length += (size_t) n_strings; // Count spaces between strings plus the \n 

    char *result = (char *)malloc(total_length * sizeof(char));
    if (result == NULL) {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    strcpy(result, command);

    for (int i = 0; i < n_strings; ++i) {
        strcat(result, " ");
        strcat(result, strings[i]);
    }

    strcat(result, "\n");

    return result;
}