static char *skip_white(char *s)
{
    if (s != NULL) {
        while (*s) {
            if (*s == '\n' || *s == '\r') {
                return NULL;
            }
            if (isspace(*s)) {
                ++s;
                continue;
            }
            return s;
        }
    }
    return NULL;
}
