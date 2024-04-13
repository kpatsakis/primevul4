EXPORTED int mboxlist_set_racls(int enabled)
{
    struct txn *tid = NULL;
    int r = 0;
    int now = !cyrusdb_fetch(mbdb, "$RACL", 5, NULL, NULL, &tid);

    if (now && !enabled) {
        syslog(LOG_NOTICE, "removing reverse acl support");
        /* remove */
        r = cyrusdb_foreach(mbdb, "$RACL", 5, NULL, racls_del_cb, &tid, &tid);
    }
    else if (enabled && !now) {
        /* add */
        struct allmb_rock mbrock = { NULL, 0, racls_add_cb, &tid };
        /* we can't use mboxlist_allmbox because it doesn't do transactions */
        syslog(LOG_NOTICE, "adding reverse acl support");
        r = cyrusdb_foreach(mbdb, "", 0, allmbox_p, allmbox_cb, &mbrock, &tid);
        if (r) {
            syslog(LOG_ERR, "ERROR: failed to add reverse acl support %s", error_message(r));
        }
        mboxlist_entry_free(&mbrock.mbentry);
        if (!r) r = cyrusdb_store(mbdb, "$RACL", 5, "", 0, &tid);
    }

    if (r)
        cyrusdb_abort(mbdb, tid);
    else
        cyrusdb_commit(mbdb, tid);

    return r;
}
