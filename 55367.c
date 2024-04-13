static void mboxlist_closesubs(struct db *sub)
{
    cyrusdb_close(sub);
}
