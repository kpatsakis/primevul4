mboxlist_opensubs(const char *userid,
                  struct db **ret)
{
    int r = 0, flags;
    char *subsfname;

    /* Build subscription list filename */
    subsfname = user_hash_subs(userid);

    flags = CYRUSDB_CREATE;
    if (config_getswitch(IMAPOPT_IMPROVED_MBOXLIST_SORT)) {
        flags |= CYRUSDB_MBOXSORT;
    }

    r = cyrusdb_open(SUBDB, subsfname, flags, ret);
    if (r != CYRUSDB_OK) {
        r = IMAP_IOERROR;
    }
    free(subsfname);

    return r;
}
