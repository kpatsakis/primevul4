mboxlist_sync_setacls(const char *name, const char *newacl)
{
    mbentry_t *mbentry = NULL;
    int r;
    struct txn *tid = NULL;

    /* 1. Start Transaction */
    /* lookup the mailbox to make sure it exists and get its acl */
    do {
        r = mboxlist_mylookup(name, &mbentry, &tid, 1);
    } while(r == IMAP_AGAIN);

    /* Can't do this to an in-transit or reserved mailbox */
    if (!r && mbentry->mbtype & (MBTYPE_MOVING | MBTYPE_RESERVE | MBTYPE_DELETED)) {
        r = IMAP_MAILBOX_NOTSUPPORTED;
    }

    /* 2. Set DB Entry */
    if (!r) {
        /* ok, change the database */
        free(mbentry->acl);
        mbentry->acl = xstrdupnull(newacl);

        r = mboxlist_update_entry(name, mbentry, &tid);

        if (r) {
            syslog(LOG_ERR, "DBERROR: error updating acl %s: %s",
                   name, cyrusdb_strerror(r));
            r = IMAP_IOERROR;
        }
    }

    /* 3. Commit transaction */
    if (!r) {
        r = cyrusdb_commit(mbdb, tid);
        if (r) {
            syslog(LOG_ERR, "DBERROR: failed on commit %s: %s",
                   name, cyrusdb_strerror(r));
            r = IMAP_IOERROR;
        }
        tid = NULL;
    }

    /* 4. Change mupdate entry  */
    if (!r && config_mupdate_server) {
        mupdate_handle *mupdate_h = NULL;
        /* commit the update to MUPDATE */
        char buf[MAX_PARTITION_LEN + HOSTNAME_SIZE + 2];
        sprintf(buf, "%s!%s", config_servername, mbentry->partition);

        r = mupdate_connect(config_mupdate_server, NULL, &mupdate_h, NULL);
        if (r) {
            syslog(LOG_ERR,
                   "cannot connect to mupdate server for syncacl on '%s'",
                   name);
        } else {
            r = mupdate_activate(mupdate_h, name, buf, newacl);
            if(r) {
                syslog(LOG_ERR,
                       "MUPDATE: can't update mailbox entry for '%s'",
                       name);
            }
        }
        mupdate_disconnect(&mupdate_h);
    }

    if (r && tid) {
        /* if we are mid-transaction, abort it! */
        int r2 = cyrusdb_abort(mbdb, tid);
        if (r2) {
            syslog(LOG_ERR,
                   "DBERROR: error aborting txn in sync_setacls %s: %s",
                   name, cyrusdb_strerror(r2));
        }
    }

    mboxlist_entry_free(&mbentry);

    return r;
}
