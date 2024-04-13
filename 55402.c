static int mboxlist_read(const char *name, const char **dataptr, size_t *datalenptr,
                         struct txn **tid, int wrlock)
{
    int namelen = strlen(name);
    int r;

    if (!namelen)
        return IMAP_MAILBOX_NONEXISTENT;

    if (wrlock) {
        r = cyrusdb_fetchlock(mbdb, name, namelen, dataptr, datalenptr, tid);
    } else {
        r = cyrusdb_fetch(mbdb, name, namelen, dataptr, datalenptr, tid);
    }

    switch (r) {
    case CYRUSDB_OK:
        /* no entry required, just checking if it exists */
        return 0;
        break;

    case CYRUSDB_AGAIN:
        return IMAP_AGAIN;
        break;

    case CYRUSDB_NOTFOUND:
        return IMAP_MAILBOX_NONEXISTENT;
        break;

    default:
        syslog(LOG_ERR, "DBERROR: error fetching mboxlist %s: %s",
               name, cyrusdb_strerror(r));
        return IMAP_IOERROR;
        break;
    }

    /* never get here */
}
