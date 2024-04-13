EXPORTED int mboxlist_setacl(const struct namespace *namespace __attribute__((unused)),
                    const char *name,
                    const char *identifier, const char *rights,
                    int isadmin, const char *userid,
                    const struct auth_state *auth_state)
{
    mbentry_t *mbentry = NULL;
    int r;
    int myrights;
    int mode = ACL_MODE_SET;
    int isusermbox = 0;
    int isidentifiermbox = 0;
    int anyoneuseracl = 1;
    int ensure_owner_rights = 0;
    int mask;
    const char *mailbox_owner = NULL;
    struct mailbox *mailbox = NULL;
    char *newacl = NULL;
    struct txn *tid = NULL;

    /* round trip identifier to potentially strip domain */
    mbname_t *mbname = mbname_from_userid(identifier);
    /* XXX - enforce cross domain restrictions */
    identifier = mbname_userid(mbname);

    /* checks if the mailbox belongs to the user who is trying to change the
       access rights */
    if (mboxname_userownsmailbox(userid, name))
        isusermbox = 1;
    anyoneuseracl = config_getswitch(IMAPOPT_ANYONEUSERACL);

    /* checks if the identifier is the mailbox owner */
    if (mboxname_userownsmailbox(identifier, name))
        isidentifiermbox = 1;

    /* who is the mailbox owner? */
    if (isusermbox) {
        mailbox_owner = userid;
    }
    else if (isidentifiermbox) {
        mailbox_owner = identifier;
    }

    /* ensure the access rights if the folder owner is the current user or
       the identifier */
    ensure_owner_rights = isusermbox || isidentifiermbox;

    /* 1. Start Transaction */
    /* lookup the mailbox to make sure it exists and get its acl */
    do {
        r = mboxlist_mylookup(name, &mbentry, &tid, 1);
    } while(r == IMAP_AGAIN);

    /* Can't do this to an in-transit or reserved mailbox */
    if (!r && mbentry->mbtype & (MBTYPE_MOVING | MBTYPE_RESERVE | MBTYPE_DELETED)) {
        r = IMAP_MAILBOX_NOTSUPPORTED;
    }

    /* if it is not a remote mailbox, we need to unlock the mailbox list,
     * lock the mailbox, and re-lock the mailboxes list */
    /* we must do this to obey our locking rules */
    if (!r && !(mbentry->mbtype & MBTYPE_REMOTE)) {
        cyrusdb_abort(mbdb, tid);
        tid = NULL;
        mboxlist_entry_free(&mbentry);

        /* open & lock mailbox header */
        r = mailbox_open_iwl(name, &mailbox);

        if (!r) {
            do {
                /* lookup the mailbox to make sure it exists and get its acl */
                r = mboxlist_mylookup(name, &mbentry, &tid, 1);
            } while (r == IMAP_AGAIN);
        }

        if(r) goto done;
    }

    /* 2. Check Rights */
    if (!r && !isadmin) {
        myrights = cyrus_acl_myrights(auth_state, mbentry->acl);
        if (!(myrights & ACL_ADMIN)) {
            r = (myrights & ACL_LOOKUP) ?
                IMAP_PERMISSION_DENIED : IMAP_MAILBOX_NONEXISTENT;
            goto done;
        }
    }

    /* 2.1 Only admin user can set 'anyone' rights if config says so */
    if (!r && !isadmin && !anyoneuseracl && !strncmp(identifier, "anyone", 6)) {
      r = IMAP_PERMISSION_DENIED;
      goto done;
    }

    /* 3. Set DB Entry */
    if(!r) {
        /* Make change to ACL */
        newacl = xstrdup(mbentry->acl);
        if (rights && *rights) {
            /* rights are present and non-empty */
            mode = ACL_MODE_SET;
            if (*rights == '+') {
                rights++;
                mode = ACL_MODE_ADD;
            }
            else if (*rights == '-') {
                rights++;
                mode = ACL_MODE_REMOVE;
            }
            /* do not allow non-admin user to remove the admin rights from mailbox owner */
            if (!isadmin && isidentifiermbox && mode != ACL_MODE_ADD) {
                int has_admin_rights = mboxlist_have_admin_rights(rights);
                if ((has_admin_rights && mode == ACL_MODE_REMOVE) ||
                   (!has_admin_rights && mode != ACL_MODE_REMOVE)) {
                    syslog(LOG_ERR, "Denied removal of admin rights on "
                           "folder \"%s\" (owner: %s) by user \"%s\"", name,
                           mailbox_owner, userid);
                    r = IMAP_PERMISSION_DENIED;
                    goto done;
                }
            }

            r = cyrus_acl_strtomask(rights, &mask);

            if (!r && cyrus_acl_set(&newacl, identifier, mode, mask,
                                    ensure_owner_rights ? mboxlist_ensureOwnerRights : 0,
                                    (void *)mailbox_owner)) {
                r = IMAP_INVALID_IDENTIFIER;
            }
        } else {
            /* do not allow to remove the admin rights from mailbox owner */
            if (!isadmin && isidentifiermbox) {
                syslog(LOG_ERR, "Denied removal of admin rights on "
                       "folder \"%s\" (owner: %s) by user \"%s\"", name,
                       mailbox_owner, userid);
                r = IMAP_PERMISSION_DENIED;
                goto done;
            }

            if (cyrus_acl_remove(&newacl, identifier,
                                 ensure_owner_rights ? mboxlist_ensureOwnerRights : 0,
                                 (void *)mailbox_owner)) {
                r = IMAP_INVALID_IDENTIFIER;
            }
        }
    }

    if (!r) {
        /* ok, change the database */
        free(mbentry->acl);
        mbentry->acl = xstrdupnull(newacl);

        r = mboxlist_update_entry(name, mbentry, &tid);

        if (r) {
            syslog(LOG_ERR, "DBERROR: error updating acl %s: %s",
                   name, cyrusdb_strerror(r));
            r = IMAP_IOERROR;
        }

        /* send a AclChange event notification */
        struct mboxevent *mboxevent = mboxevent_new(EVENT_ACL_CHANGE);
        mboxevent_extract_mailbox(mboxevent, mailbox);
        mboxevent_set_acl(mboxevent, identifier, rights);
        mboxevent_set_access(mboxevent, NULL, NULL, userid, mailbox->name, 0);

        mboxevent_notify(&mboxevent);
        mboxevent_free(&mboxevent);

    }

    /* 4. Change backup copy (cyrus.header) */
    /* we already have it locked from above */
    if (!r && !(mbentry->mbtype & MBTYPE_REMOTE)) {
        mailbox_set_acl(mailbox, newacl, 1);
        /* want to commit immediately to ensure ordering */
        r = mailbox_commit(mailbox);
    }

    /* 5. Commit transaction */
    if (!r) {
        if((r = cyrusdb_commit(mbdb, tid)) != 0) {
            syslog(LOG_ERR, "DBERROR: failed on commit: %s",
                   cyrusdb_strerror(r));
            r = IMAP_IOERROR;
        }
        tid = NULL;
    }

    /* 6. Change mupdate entry  */
    if (!r && config_mupdate_server) {
        mupdate_handle *mupdate_h = NULL;
        /* commit the update to MUPDATE */
        char buf[MAX_PARTITION_LEN + HOSTNAME_SIZE + 2];

        snprintf(buf, sizeof(buf), "%s!%s", config_servername, mbentry->partition);

        r = mupdate_connect(config_mupdate_server, NULL, &mupdate_h, NULL);
        if(r) {
            syslog(LOG_ERR,
                   "cannot connect to mupdate server for setacl on '%s'",
                   name);
        } else {
            r = mupdate_activate(mupdate_h, name, buf, newacl);
            if(r) {
                syslog(LOG_ERR,
                       "MUPDATE: can't update mailbox entry for '%s'",
                       name);
            }
        }
        mupdate_disconnect(&mupdate_h);
    }

  done:
    if (r && tid) {
        /* if we are mid-transaction, abort it! */
        int r2 = cyrusdb_abort(mbdb, tid);
        if (r2) {
            syslog(LOG_ERR,
                   "DBERROR: error aborting txn in mboxlist_setacl: %s",
                   cyrusdb_strerror(r2));
        }
    }
    mailbox_close(&mailbox);
    free(newacl);
    mboxlist_entry_free(&mbentry);
    mbname_free(&mbname);

    return r;
}
