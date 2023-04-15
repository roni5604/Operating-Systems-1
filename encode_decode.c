#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <encode/decode> <codec> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *mode = argv[1];
    const char *codec = argv[2];
    char *message = argv[3];

    char lib_name[20];
    snprintf(lib_name, sizeof(lib_name), "lib%s.so", codec);

    void *library = dlopen(lib_name, RTLD_LAZY);
    if (!library) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    char function_name[20];
    snprintf(function_name, sizeof(function_name), "%s_%s", mode, codec);

    void (*process)(char *) = dlsym(library, function_name);
    if (!process) {
        fprintf(stderr, "Error loading function: %s\n", dlerror());
        dlclose(library);
        exit(EXIT_FAILURE);
    }

    process(message);

    printf("%s\n", message);

    dlclose(library);

    return 0;
}
