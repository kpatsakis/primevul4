EXPORTED int mboxlist_usermboxtree(const char *userid, mboxlist_cb *proc,
                                   void *rock, int flags)
{
    char *inbox = mboxname_user_mbox(userid, 0);
    int r = mboxlist_mboxtree(inbox, proc, rock, flags);

    if (flags & MBOXTREE_PLUS_RACL) {
        struct allmb_rock mbrock = { NULL, flags, proc, rock };
        /* we're using reverse ACLs */
        struct buf buf = BUF_INITIALIZER;
        strarray_t matches = STRARRAY_INITIALIZER;

        /* user items */
        mboxlist_racl_key(1, userid, NULL, &buf);
        /* this is the prefix */
        struct raclrock raclrock = { buf.len, &matches };
        /* we only need to look inside the prefix still, but we keep the length
         * in raclrock pointing to the start of the mboxname part of the key so
         * we get correct names in matches */
        r = cyrusdb_foreach(mbdb, buf.s, buf.len, NULL, racl_cb, &raclrock, NULL);
        buf_reset(&buf);

        /* shared items */
        mboxlist_racl_key(0, userid, NULL, &buf);
        raclrock.prefixlen = buf.len;
        if (!r) r = cyrusdb_foreach(mbdb, buf.s, buf.len, NULL, racl_cb, &raclrock, NULL);

        /* XXX - later we need to sort the array when we've added groups */
        int i;
        for (i = 0; !r && i < strarray_size(&matches); i++) {
            const char *mboxname = strarray_nth(&matches, i);
            r = cyrusdb_forone(mbdb, mboxname, strlen(mboxname), allmbox_p, allmbox_cb, &mbrock, 0);
        }
        buf_free(&buf);
        strarray_fini(&matches);
        mboxlist_entry_free(&mbrock.mbentry);
    }

    free(inbox);
    return r;
}
