static char *skip_idf(char *start, char out_idf[256])
{
    char *s;
    char c;

    s = skip_white(start);
    if (s == NULL) {
        return NULL;
    }
    start = s;

    while (*s) {
        if (isalpha(*s) || *s == '_') {
            ++s;
            continue;
        }
        break;
    }
    c = *s;
    *s = 0;
    strncpy(out_idf, start, 255);
    *s = c;
    return s;
}
