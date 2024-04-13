EXPORTED int mboxlist_deleteremote(const char *name, struct txn **in_tid)
{
    int r;
    struct txn **tid;
    struct txn *lcl_tid = NULL;
    mbentry_t *mbentry = NULL;

    if(in_tid) {
        tid = in_tid;
    } else {
        tid = &lcl_tid;
    }

 retry:
    r = mboxlist_mylookup(name, &mbentry, tid, 1);
    switch (r) {
    case 0:
        break;

    case IMAP_MAILBOX_NONEXISTENT:
        r = 0;
        break;

    case IMAP_AGAIN:
        goto retry;
        break;

    default:
        goto done;
    }

    if (mbentry && (mbentry->mbtype & MBTYPE_REMOTE) && !mbentry->server) {
        syslog(LOG_ERR,
               "mboxlist_deleteremote called on non-remote mailbox: %s",
               name);
        goto done;
    }

    r = mboxlist_update_entry(name, NULL, tid);
    if (r) {
        syslog(LOG_ERR, "DBERROR: error deleting %s: %s",
               name, cyrusdb_strerror(r));
        r = IMAP_IOERROR;
    }

    /* commit db operations, but only if we weren't passed a transaction */
    if (!in_tid) {
        r = cyrusdb_commit(mbdb, *tid);
        if (r) {
            syslog(LOG_ERR, "DBERROR: failed on commit: %s",
                   cyrusdb_strerror(r));
            r = IMAP_IOERROR;
        }
        tid = NULL;
    }

 done:
    if (r && !in_tid && tid) {
        /* Abort the transaction if it is still in progress */
        cyrusdb_abort(mbdb, *tid);
    }

    return r;
}
