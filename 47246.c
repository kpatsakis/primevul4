int CLASS fscanf(FILE *stream, const char *format, void *ptr) {
    int count = ::fscanf(stream, format, ptr);
    if ( count != 1 )
        dcraw_message(DCRAW_WARNING, "%s: fscanf %d != 1\n",
                ifname_display, count);
    return 1;
}
