char *CLASS fgets(char *s, int size, FILE *stream) {
    char *str = ::fgets(s, size, stream);
    if (str == NULL) {
        if (eofCount < 10)
            dcraw_message(DCRAW_VERBOSE, "%s: fgets returned NULL\n",
                    ifname_display);
        if (eofCount == 10)
            dcraw_message(DCRAW_VERBOSE, "%s: fgets eof reached 10 times\n",
                    ifname_display);
        eofCount++;
    }
    if (stream==ifp) ifpProgress(strlen(s));
    return str;
}
