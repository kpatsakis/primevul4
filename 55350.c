static int _find_uniqueid(const mbentry_t *mbentry, void *rock) {
    struct _find_uniqueid_data *d = (struct _find_uniqueid_data *) rock;
    int r = 0;
    if (!strcmp(d->uniqueid, mbentry->uniqueid)) {
        d->mboxname = xstrdup(mbentry->name);
        r = CYRUSDB_DONE;
    }
    return r;
}
