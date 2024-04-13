size_t CLASS fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t num = ::fread(ptr, size, nmemb, stream);
    if ( num != nmemb ) {
        if (eofCount < 10)
            dcraw_message(DCRAW_VERBOSE, "%s: fread %d != %d\n",
                    ifname_display, num, nmemb);
        if (eofCount == 10)
            dcraw_message(DCRAW_VERBOSE, "%s: fread eof reached 10 times\n",
                    ifname_display);
        eofCount++;
    }
    if (stream==ifp) ifpProgress(size*nmemb);
    return num;
}
