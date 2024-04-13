static int _find_specialuse(const mbentry_t *mbentry, void *rock)
{
    struct _find_specialuse_data *d = (struct _find_specialuse_data *)rock;
    struct buf attrib = BUF_INITIALIZER;

    annotatemore_lookup(mbentry->name, "/specialuse", d->userid, &attrib);

    if (attrib.len) {
        strarray_t *uses = strarray_split(buf_cstring(&attrib), " ", 0);
        if (strarray_find_case(uses, d->use, 0) >= 0)
            d->mboxname = xstrdup(mbentry->name);
        strarray_free(uses);
    }

    buf_free(&attrib);

    if (d->mboxname) return CYRUSDB_DONE;
    return 0;
}
