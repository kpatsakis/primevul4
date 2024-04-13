static int mboxlist_createmailbox_full(const char *mboxname, int mbtype,
                                const char *partition,
                                int isadmin, const char *userid,
                                const struct auth_state *auth_state,
                                int options, unsigned uidvalidity,
                                modseq_t highestmodseq,
                                const char *copyacl, const char *uniqueid,
                                int localonly, int forceuser, int dbonly,
                                struct mailbox **mboxptr)
{
    int r;
    char *newpartition = NULL;
    char *acl = NULL;
    struct mailbox *newmailbox = NULL;
    int isremote = mbtype & MBTYPE_REMOTE;
    mbentry_t *newmbentry = NULL;

    r = mboxlist_create_namecheck(mboxname, userid, auth_state,
                                  isadmin, forceuser);
    if (r) goto done;

    if (copyacl) {
        acl = xstrdup(copyacl);
    }
    else {
        r = mboxlist_create_acl(mboxname, &acl);
        if (r) goto done;
    }

    r = mboxlist_create_partition(mboxname, partition, &newpartition);
    if (r) goto done;

    if (!dbonly && !isremote) {
        /* Filesystem Operations */
        r = mailbox_create(mboxname, mbtype, newpartition, acl, uniqueid,
                           options, uidvalidity, highestmodseq, &newmailbox);
        if (r) goto done; /* CREATE failed */
        r = mailbox_add_conversations(newmailbox);
        if (r) goto done;
    }

    /* all is well - activate the mailbox */
    newmbentry = mboxlist_entry_create();
    newmbentry->acl = xstrdupnull(acl);
    newmbentry->mbtype = mbtype;
    newmbentry->partition = xstrdupnull(newpartition);
    if (newmailbox) {
        newmbentry->uniqueid = xstrdupnull(newmailbox->uniqueid);
        newmbentry->uidvalidity = newmailbox->i.uidvalidity;
        newmbentry->foldermodseq = newmailbox->i.highestmodseq;
    }
    r = mboxlist_update_entry(mboxname, newmbentry, NULL);

    if (r) {
        syslog(LOG_ERR, "DBERROR: failed to insert to mailboxes list %s: %s",
               mboxname, cyrusdb_strerror(r));
        r = IMAP_IOERROR;
    }

    /* 9. set MUPDATE entry as commited (CRASH: commited) */
    if (!r && config_mupdate_server && !localonly) {
        mupdate_handle *mupdate_h = NULL;
        char *loc = strconcat(config_servername, "!", newpartition, (char *)NULL);

        r = mupdate_connect(config_mupdate_server, NULL, &mupdate_h, NULL);
        if (!r) r = mupdate_reserve(mupdate_h, mboxname, loc);
        if (!r) r = mupdate_activate(mupdate_h, mboxname, loc, acl);
        if (r) {
            syslog(LOG_ERR, "MUPDATE: can't commit mailbox entry for '%s'",
                   mboxname);
            mboxlist_update_entry(mboxname, NULL, 0);
        }
        if (mupdate_h) mupdate_disconnect(&mupdate_h);
        free(loc);
    }

done:
    if (newmailbox) {
        if (r) mailbox_delete(&newmailbox);
        else if (mboxptr) *mboxptr = newmailbox;
        else mailbox_close(&newmailbox);
    }

    free(acl);
    free(newpartition);
    mboxlist_entry_free(&newmbentry);

    return r;
}
