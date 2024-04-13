EXPORTED int mboxlist_mboxtree(const char *mboxname, mboxlist_cb *proc, void *rock, int flags)
{
    struct allmb_rock mbrock = { NULL, flags, proc, rock };
    int r = 0;

    if (!(flags & MBOXTREE_SKIP_ROOT)) {
        r = cyrusdb_forone(mbdb, mboxname, strlen(mboxname), allmbox_p, allmbox_cb, &mbrock, 0);
        if (r) goto done;
    }

    if (!(flags & MBOXTREE_SKIP_CHILDREN)) {
        char *prefix = strconcat(mboxname, ".", (char *)NULL);
        r = cyrusdb_foreach(mbdb, prefix, strlen(prefix), allmbox_p, allmbox_cb, &mbrock, 0);
        free(prefix);
        if (r) goto done;
    }

    if ((flags & MBOXTREE_DELETED)) {
        struct buf buf = BUF_INITIALIZER;
        const char *p = strchr(mboxname, '!');
        const char *dp = config_getstring(IMAPOPT_DELETEDPREFIX);
        if (p) {
            buf_printf(&buf, "%.*s!%s.%s", (int)(p-mboxname), mboxname, dp, p+1);
        }
        else {
            buf_printf(&buf, "%s.%s", dp, mboxname);
        }
        const char *prefix = buf_cstring(&buf);
        r = cyrusdb_foreach(mbdb, prefix, strlen(prefix), allmbox_p, allmbox_cb, &mbrock, 0);
        buf_free(&buf);
        if (r) goto done;
    }

 done:
    mboxlist_entry_free(&mbrock.mbentry);
    return r;
}
