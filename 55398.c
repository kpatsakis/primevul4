EXPORTED void mboxlist_open(const char *fname)
{
    int ret, flags;
    char *tofree = NULL;

    if (!fname)
        fname = config_getstring(IMAPOPT_MBOXLIST_DB_PATH);

    /* create db file name */
    if (!fname) {
        tofree = strconcat(config_dir, FNAME_MBOXLIST, (char *)NULL);
        fname = tofree;
    }

    flags = CYRUSDB_CREATE;
    if (config_getswitch(IMAPOPT_IMPROVED_MBOXLIST_SORT)) {
        flags |= CYRUSDB_MBOXSORT;
    }

    ret = cyrusdb_open(DB, fname, flags, &mbdb);
    if (ret != 0) {
        syslog(LOG_ERR, "DBERROR: opening %s: %s", fname,
               cyrusdb_strerror(ret));
            /* Exiting TEMPFAIL because Sendmail thinks this
               EC_OSFILE == permanent failure. */
        fatal("can't read mailboxes file", EC_TEMPFAIL);
    }

    free(tofree);

    mboxlist_dbopen = 1;
}
