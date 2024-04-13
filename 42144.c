reassemble_files_name(const char *certfile, const char *keyfile)
{
    char *ret;

    if (keyfile != NULL) {
        if (asprintf(&ret, "FILE:%s,%s", certfile, keyfile) < 0)
            return NULL;
    } else {
        if (asprintf(&ret, "FILE:%s", certfile) < 0)
            return NULL;
    }
    return ret;
}
