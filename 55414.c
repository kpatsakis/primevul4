EXPORTED int mboxlist_usersubs(const char *userid, mboxlist_cb *proc,
                               void *rock, int flags)
{
    struct db *subs = NULL;
    struct submb_rock mbrock = { NULL, userid, flags, proc, rock };
    int r = 0;

    /* open subs DB */
    r = mboxlist_opensubs(userid, &subs);
    if (r) return r;

    /* faster to do it all in a single slurp! */
    r = cyrusdb_foreach(subs, "", 0, NULL, usersubs_cb, &mbrock, 0);

    mboxlist_entry_free(&mbrock.mbentry);

    mboxlist_closesubs(subs);

    return r;
}
