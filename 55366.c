EXPORTED void mboxlist_close(void)
{
    int r;

    if (mboxlist_dbopen) {
        r = cyrusdb_close(mbdb);
        if (r) {
            syslog(LOG_ERR, "DBERROR: error closing mailboxes: %s",
                   cyrusdb_strerror(r));
        }
        mboxlist_dbopen = 0;
    }
}
