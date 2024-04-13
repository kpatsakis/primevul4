EXPORTED int mboxlist_checksub(const char *name, const char *userid)
{
    int r;
    struct db *subs;
    const char *val;
    size_t vallen;

    r = mboxlist_opensubs(userid, &subs);

    if (!r) r = cyrusdb_fetch(subs, name, strlen(name), &val, &vallen, NULL);

    mboxlist_closesubs(subs);
    return r;
}
