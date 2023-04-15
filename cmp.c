#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE *file1, *file2;
    bool verbose = false, ignore_case = false;
    int opt;

    while ((opt = getopt(argc, argv, "vi")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'i':
                ignore_case = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [-i] <file1> <file2>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind + 2 != argc) {
        fprintf(stderr, "Usage: %s [-v] [-i] <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    file1 = fopen(argv[optind], "r");
    file2 = fopen(argv[optind + 1], "r");

    if (file1 == NULL || file2 == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int result = 0;
    int ch1, ch2;

    do {
        ch1 = fgetc(file1);
        ch2 = fgetc(file2);

        if (ignore_case) {
            ch1 = tolower(ch1);
            ch2 = tolower(ch2);
        }

        if (ch1 != ch2) {
            result = 1;
            break;
        }
    } while (ch1 != EOF && ch2 != EOF);

    fclose(file1);
    fclose(file2);

    if (verbose) {
        printf("%s\n", result == 0 ? "equal" : "distinct");
    } else {
        printf("%d\n", result);
    }

    return result;
}
