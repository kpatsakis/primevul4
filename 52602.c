ktypes2str(char *s, size_t len, int nktypes, krb5_enctype *ktype)
{
    int i;
    char stmp[D_LEN(krb5_enctype) + 1];
    char *p;

    if (nktypes < 0
        || len < (sizeof(" etypes {...}") + D_LEN(int))) {
        *s = '\0';
        return;
    }

    snprintf(s, len, "%d etypes {", nktypes);
    for (i = 0; i < nktypes; i++) {
        snprintf(stmp, sizeof(stmp), "%s%ld", i ? " " : "", (long)ktype[i]);
        if (strlen(s) + strlen(stmp) + sizeof("}") > len)
            break;
        strlcat(s, stmp, len);
    }
    if (i < nktypes) {
        /*
         * We broke out of the loop. Try to truncate the list.
         */
        p = s + strlen(s);
        while (p - s + sizeof("...}") > len) {
            while (p > s && *p != ' ' && *p != '{')
                *p-- = '\0';
            if (p > s && *p == ' ') {
                *p-- = '\0';
                continue;
            }
        }
        strlcat(s, "...", len);
    }
    strlcat(s, "}", len);
