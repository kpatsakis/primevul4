EXPORTED int mboxlist_insertremote(mbentry_t *mbentry,
                          struct txn **txn)
{
    int r = 0;

    if (mbentry->server) {
        /* remote mailbox */
        if (config_mupdate_config == IMAP_ENUM_MUPDATE_CONFIG_UNIFIED &&
            !strcasecmp(mbentry->server, config_servername)) {
            /* its on our server, make it a local mailbox */
            mbentry->mbtype &= ~MBTYPE_REMOTE;
            mbentry->server = NULL;
        }
        else {
            /* make sure it's a remote mailbox */
            mbentry->mbtype |= MBTYPE_REMOTE;
        }
    }

    /* database put */
    r = mboxlist_update_entry(mbentry->name, mbentry, txn);

    switch (r) {
    case CYRUSDB_OK:
        break;
    case CYRUSDB_AGAIN:
        abort(); /* shouldn't happen ! */
        break;
    default:
        syslog(LOG_ERR, "DBERROR: error updating database %s: %s",
               mbentry->name, cyrusdb_strerror(r));
        r = IMAP_IOERROR;
        break;
    }

    return r;
}
