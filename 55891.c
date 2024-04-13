static char *skip_int(char *start, int *out_n)
{
    char *s;
    char c;

    *out_n = 0;

    s = skip_white(start);
    if (s == NULL) {
        return NULL;
    }
    start = s;

    while (*s) {
        if (!isdigit(*s)) {
            break;
        }
        ++s;
    }
    c = *s;
    *s = 0;
    *out_n = atoi(start);
    *s = c;
    return s;
}
