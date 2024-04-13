EXPORTED int mboxlist_update(mbentry_t *mbentry, int localonly)
{
    int r = 0, r2 = 0;
    struct txn *tid = NULL;

    r = mboxlist_update_entry(mbentry->name, mbentry, &tid);

    if (!r)
        mboxname_setmodseq(mbentry->name, mbentry->foldermodseq, mbentry->mbtype, /*dofolder*/1);

    /* commit the change to mupdate */
    if (!r && !localonly && config_mupdate_server) {
        mupdate_handle *mupdate_h = NULL;

        r = mupdate_connect(config_mupdate_server, NULL, &mupdate_h, NULL);
        if (r) {
            syslog(LOG_ERR,
                   "cannot connect to mupdate server for update of '%s'",
                   mbentry->name);
        } else {
            char *location = strconcat(config_servername, "!",
                                       mbentry->partition, (char *)NULL);
            r = mupdate_activate(mupdate_h, mbentry->name,
                                 location, mbentry->acl);
            free(location);
            if (r) {
                syslog(LOG_ERR,
                       "MUPDATE: can't update mailbox entry for '%s'",
                       mbentry->name);
            }
        }
        mupdate_disconnect(&mupdate_h);
    }

    if (tid) {
        if (r) {
            r2 = cyrusdb_abort(mbdb, tid);
        } else {
            r2 = cyrusdb_commit(mbdb, tid);
        }
    }

    if (r2) {
        syslog(LOG_ERR, "DBERROR: error %s txn in mboxlist_update: %s",
               r ? "aborting" : "commiting", cyrusdb_strerror(r2));
    }

    return r;
}
