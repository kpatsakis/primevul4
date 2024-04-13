static int usersubs_cb(void *rock, const char *key, size_t keylen,
                      const char *data __attribute__((unused)),
                      size_t datalen __attribute__((unused)))
{
    struct submb_rock *mbrock = (struct submb_rock *) rock;
    char mboxname[MAX_MAILBOX_NAME+1];
    int r;

    /* free previous record */
    mboxlist_entry_free(&mbrock->mbentry);

    snprintf(mboxname, MAX_MAILBOX_NAME, "%.*s", (int) keylen, key);

    if ((mbrock->flags & MBOXTREE_SKIP_PERSONAL) &&
        mboxname_userownsmailbox(mbrock->userid, mboxname)) return 0;

    r = mboxlist_lookup(mboxname, &mbrock->mbentry, NULL);
    if (r) {
        syslog(LOG_INFO, "mboxlist_lookup(%s) failed: %s",
               mboxname, error_message(r));
        return r;
    }

    return mbrock->proc(mbrock->mbentry, mbrock->rock);
}
