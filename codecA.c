#include <ctype.h>

void encode_codecA(char *str) {
    while (*str) {
        if (isupper(*str))
            *str = tolower(*str);
        else if (islower(*str))
            *str = toupper(*str);
        ++str;
    }
}

void decode_codecA(char *str) {
    encode_codecA(str); // Encoding and decoding are the same for codecA
}
