EXPORTED int mboxlist_allmbox(const char *prefix, mboxlist_cb *proc, void *rock, int incdel)
{
    struct allmb_rock mbrock = { NULL, 0, proc, rock };
    int r = 0;

    if (incdel) mbrock.flags |= MBOXTREE_TOMBSTONES;
    if (!prefix) prefix = "";

    r = cyrusdb_foreach(mbdb, prefix, strlen(prefix),
                        allmbox_p, allmbox_cb, &mbrock, 0);

    mboxlist_entry_free(&mbrock.mbentry);

    return r;
}
