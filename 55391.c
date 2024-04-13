EXPORTED void mboxlist_init(int myflags)
{
    if (myflags & MBOXLIST_SYNC) {
        cyrusdb_sync(DB);
    }
}
