void encode_codecB(char *str) {
    while (*str) {
        *str = *str + 3;
        ++str;
    }
}

void decode_codecB(char *str) {
    while (*str) {
        *str = *str - 3;
        ++str;
    }
}
