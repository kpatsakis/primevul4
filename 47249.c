size_t CLASS fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t num = ::fwrite(ptr, size, nmemb, stream);
    if ( num != nmemb )
        dcraw_message(DCRAW_WARNING, "%s: fwrite %d != %d\n",
                ifname_display, num, nmemb);
    return num;
}
