EXPORTED int mboxlist_changesub(const char *name, const char *userid,
                                const struct auth_state *auth_state,
                                int add, int force, int notify)
{
    mbentry_t *mbentry = NULL;
    int r;
    struct db *subs;
    struct mboxevent *mboxevent;

    if ((r = mboxlist_opensubs(userid, &subs)) != 0) {
        return r;
    }

    if (add && !force) {
        /* Ensure mailbox exists and can be seen by user */
        if ((r = mboxlist_lookup(name, &mbentry, NULL))!=0) {
            mboxlist_closesubs(subs);
            return r;
        }
        if ((cyrus_acl_myrights(auth_state, mbentry->acl) & ACL_LOOKUP) == 0) {
            mboxlist_closesubs(subs);
            mboxlist_entry_free(&mbentry);
            return IMAP_MAILBOX_NONEXISTENT;
        }
    }

    if (add) {
        r = cyrusdb_store(subs, name, strlen(name), "", 0, NULL);
    } else {
        r = cyrusdb_delete(subs, name, strlen(name), NULL, 0);
        /* if it didn't exist, that's ok */
        if (r == CYRUSDB_EXISTS) r = CYRUSDB_OK;
    }

    switch (r) {
    case CYRUSDB_OK:
        r = 0;
        break;

    default:
        r = IMAP_IOERROR;
        break;
    }

    sync_log_subscribe(userid, name);
    mboxlist_closesubs(subs);
    mboxlist_entry_free(&mbentry);

    /* prepare a MailboxSubscribe or MailboxUnSubscribe event notification */
    if (notify && r == 0) {
        mboxevent = mboxevent_new(add ? EVENT_MAILBOX_SUBSCRIBE :
                                        EVENT_MAILBOX_UNSUBSCRIBE);

        mboxevent_set_access(mboxevent, NULL, NULL, userid, name, 1);
        mboxevent_notify(&mboxevent);
        mboxevent_free(&mboxevent);
    }

    return r;
}
