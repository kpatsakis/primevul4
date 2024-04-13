static int mboxlist_mylookup(const char *name,
                             mbentry_t **mbentryptr,
                             struct txn **tid, int wrlock)
{
    int r;
    const char *data;
    size_t datalen;

    r = mboxlist_read(name, &data, &datalen, tid, wrlock);
    if (r) return r;

    return mboxlist_parse_entry(mbentryptr, name, 0, data, datalen);
}
