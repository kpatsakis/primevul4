static size_t mod_strcspn(const char *string, const char *reject)
{
    int i, j;

    for (i = 0; string && string[i]; i++) {
        if (string[i] == '/' && string[i+1] == '*') {
            i += 2;
            while ( string && string[i] && (string[i] != '*' || string[i+1] != '/') )
                i++;
            i++;
        } else if (string[i] == '/' && string[i+1] == '/') {
            i += 2;
            while ( string && string[i] && string[i] != '\n' )
                i++;
        } else {
            for (j = 0; reject && reject[j]; j++) {
                if (string[i] == reject[j])
                    break;
            }
            if (reject && reject[j])
                break;
        }
    }
    return i;
}
