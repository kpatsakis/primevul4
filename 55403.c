EXPORTED int mboxlist_renamemailbox(const char *oldname, const char *newname,
                           const char *partition, unsigned uidvalidity,
                           int isadmin, const char *userid,
                           const struct auth_state *auth_state,
                           struct mboxevent *mboxevent,
                           int local_only, int forceuser, int ignorequota)
{
    int r;
    int mupdatecommiterror = 0;
    long myrights;
    int isusermbox = 0; /* Are we renaming someone's inbox */
    int partitionmove = 0;
    struct mailbox *oldmailbox = NULL;
    struct mailbox *newmailbox = NULL;
    struct txn *tid = NULL;
    const char *root = NULL;
    char *newpartition = NULL;
    mupdate_handle *mupdate_h = NULL;
    mbentry_t *newmbentry = NULL;

    /* 1. open mailbox */
    r = mailbox_open_iwl(oldname, &oldmailbox);
    if (r) return r;

    myrights = cyrus_acl_myrights(auth_state, oldmailbox->acl);

    /* check the ACLs up-front */
    if (!isadmin) {
        if (!(myrights & ACL_DELETEMBOX)) {
            r = (myrights & ACL_LOOKUP) ?
                IMAP_PERMISSION_DENIED : IMAP_MAILBOX_NONEXISTENT;
            goto done;
        }
    }

    /* 2. verify valid move */
    /* XXX - handle remote mailbox */

    /* special case: same mailbox, must be a partition move */
    if (!strcmp(oldname, newname)) {
        const char *oldpath = mailbox_datapath(oldmailbox, 0);

        /* Only admin can move mailboxes between partitions */
        if (!isadmin) {
            r = IMAP_PERMISSION_DENIED;
            goto done;
        }

        /* No partition, we're definitely not moving anywhere */
        if (!partition) {
            r = IMAP_MAILBOX_EXISTS;
            goto done;
        }

        /* let mupdate code below know it was a partition move */
        partitionmove = 1;

        /* this is OK because it uses a different static buffer */
        root = config_partitiondir(partition);
        if (!root) {
            r = IMAP_PARTITION_UNKNOWN;
            goto done;
        }
        if (!strncmp(root, oldpath, strlen(root)) &&
            oldpath[strlen(root)] == '/') {
            /* partitions are the same or share common prefix */
            r = IMAP_MAILBOX_EXISTS;
            goto done;
        }

        /* NOTE: this is a rename to the same mailbox name on a
         * different partition.  This is a pretty filthy hack,
         * which should be handled by having four totally different
         * codepaths: INBOX -> INBOX.foo, user rename, regular rename
         * and of course this one, partition move */
        newpartition = xstrdup(partition);
        r = mailbox_copy_files(oldmailbox, newpartition, newname, oldmailbox->uniqueid);
        if (r) goto done;
        newmbentry = mboxlist_entry_create();
        newmbentry->mbtype = oldmailbox->mbtype;
        newmbentry->partition = xstrdupnull(newpartition);
        newmbentry->acl = xstrdupnull(oldmailbox->acl);
        newmbentry->uidvalidity = oldmailbox->i.uidvalidity;
        newmbentry->uniqueid = xstrdupnull(oldmailbox->uniqueid);
        newmbentry->foldermodseq = oldmailbox->i.highestmodseq; /* bump regardless, it's rare */

        r = mboxlist_update_entry(newname, newmbentry, &tid);
        if (r) goto done;

        /* skip ahead to the commit */
        goto dbdone;
    }

    if (!isadmin) {
        r = _rename_check_specialuse(oldname, newname);
        if (r) goto done;
    }

    /* RENAME of some user's INBOX */
    if (mboxname_isusermailbox(oldname, 1)) {
        if (mboxname_isdeletedmailbox(newname, NULL)) {
            /* delete user is OK */
        }
        else if (mboxname_isusermailbox(newname, 1)) {
            /* user rename is depends on config */
            if (!config_getswitch(IMAPOPT_ALLOWUSERMOVES)) {
                r = IMAP_MAILBOX_NOTSUPPORTED;
                goto done;
            }
        }
        else if (mboxname_userownsmailbox(userid, oldname) &&
                 mboxname_userownsmailbox(userid, newname)) {
            /* Special case of renaming inbox */
            isusermbox = 1;
        }
        else {
            /* Everything else is bogus */
            r = IMAP_MAILBOX_NOTSUPPORTED;
            goto done;
        }
    }

    r = mboxlist_create_namecheck(newname, userid, auth_state,
                                  isadmin, forceuser);
    if (r) goto done;

    r = mboxlist_create_partition(newname, partition, &newpartition);
    if (r) goto done;

    if (!newpartition) newpartition = xstrdup(config_defpartition);

    /* keep uidvalidity on rename unless specified */
    if (!uidvalidity)
        uidvalidity = oldmailbox->i.uidvalidity;

    /* Rename the actual mailbox */
    r = mailbox_rename_copy(oldmailbox, newname, newpartition, uidvalidity,
                            isusermbox ? userid : NULL, ignorequota,
                            &newmailbox);

    if (r) goto done;

    syslog(LOG_INFO, "Rename: %s -> %s", oldname, newname);

    /* create new entry */
    newmbentry = mboxlist_entry_create();
    newmbentry->name = xstrdupnull(newmailbox->name);
    newmbentry->mbtype = newmailbox->mbtype;
    newmbentry->partition = xstrdupnull(newmailbox->part);
    newmbentry->acl = xstrdupnull(newmailbox->acl);
    newmbentry->uidvalidity = newmailbox->i.uidvalidity;
    newmbentry->uniqueid = xstrdupnull(newmailbox->uniqueid);
    newmbentry->foldermodseq = newmailbox->i.highestmodseq;

    do {
        r = 0;

        /* delete the old entry */
        if (!isusermbox) {
            /* store a DELETED marker */
            mbentry_t *oldmbentry = mboxlist_entry_create();
            oldmbentry->name = xstrdupnull(oldmailbox->name);
            oldmbentry->mbtype = MBTYPE_DELETED;
            oldmbentry->uidvalidity = oldmailbox->i.uidvalidity;
            oldmbentry->uniqueid = xstrdupnull(oldmailbox->uniqueid);
            oldmbentry->foldermodseq = mailbox_modseq_dirty(oldmailbox);

            r = mboxlist_update_entry(oldname, oldmbentry, &tid);

            mboxlist_entry_free(&oldmbentry);
        }

        /* create a new entry */
        if (!r) {
            r = mboxlist_update_entry(newname, newmbentry, &tid);
        }

        switch (r) {
        case 0: /* success */
            break;
        case CYRUSDB_AGAIN:
            tid = NULL;
            break;
        default:
            syslog(LOG_ERR, "DBERROR: rename failed on store %s %s: %s",
                   oldname, newname, cyrusdb_strerror(r));
            r = IMAP_IOERROR;
            goto done;
            break;
        }
    } while (r == CYRUSDB_AGAIN);

 dbdone:

    /* 3. Commit transaction */
    r = cyrusdb_commit(mbdb, tid);
    tid = NULL;
    if (r) {
        syslog(LOG_ERR, "DBERROR: rename failed on commit %s %s: %s",
               oldname, newname, cyrusdb_strerror(r));
        r = IMAP_IOERROR;
        goto done;
    }

    if (!local_only && config_mupdate_server) {
        /* commit the mailbox in MUPDATE */
        char *loc = strconcat(config_servername, "!", newpartition, (char *)NULL);

        r = mupdate_connect(config_mupdate_server, NULL, &mupdate_h, NULL);
        if (!partitionmove) {
            if (!r && !isusermbox)
                r = mupdate_delete(mupdate_h, oldname);
            if (!r) r = mupdate_reserve(mupdate_h, newname, loc);
        }
        if (!r) r = mupdate_activate(mupdate_h, newname, loc, newmbentry->acl);
        if (r) {
            syslog(LOG_ERR,
                   "MUPDATE: can't commit mailbox entry for '%s'",
                   newname);
            mupdatecommiterror = r;
        }
        if (mupdate_h) mupdate_disconnect(&mupdate_h);
        free(loc);
    }

 done: /* Commit or cleanup */
    if (!r && newmailbox)
        r = mailbox_commit(newmailbox);

    if (r) {
        /* rollback DB changes if it was an mupdate failure */
        if (mupdatecommiterror) {
            r = 0;

            /* recreate an old entry */
            if (!isusermbox)
                r = mboxlist_update_entry(oldname, newmbentry, &tid);

            /* delete the new entry */
            if (!r)
                r = mboxlist_update_entry(newname, NULL, &tid);

            /* Commit transaction */
            if (!r)
                r = cyrusdb_commit(mbdb, tid);

            tid = NULL;
            if (r) {
                /* XXX HOWTO repair this mess! */
                syslog(LOG_ERR, "DBERROR: failed DB rollback on mailboxrename %s %s: %s",
                       oldname, newname, cyrusdb_strerror(r));
                syslog(LOG_ERR, "DBERROR: mailboxdb on mupdate and backend ARE NOT CONSISTENT");
                syslog(LOG_ERR, "DBERROR: mailboxdb on mupdate has entry for %s, mailboxdb on backend has entry for %s and files are on the old position", oldname, newname);
                r = IMAP_IOERROR;
            } else {
                r = mupdatecommiterror;
            }
        }

        if (newmailbox) mailbox_delete(&newmailbox);
        if (partitionmove && newpartition)
            mailbox_delete_cleanup(NULL, newpartition, newname, oldmailbox->uniqueid);
        mailbox_close(&oldmailbox);
    } else {
        if (newmailbox) {
            /* prepare the event notification */
            if (mboxevent) {

                /* case of delayed delete */
                if (mboxevent->type == EVENT_MAILBOX_DELETE)
                    mboxevent_extract_mailbox(mboxevent, oldmailbox);
                else {
                    mboxevent_extract_mailbox(mboxevent, newmailbox);
                    mboxevent_extract_old_mailbox(mboxevent, oldmailbox);
                }

                mboxevent_set_access(mboxevent, NULL, NULL, userid, newmailbox->name, 1);
            }

            /* log the rename before we close either mailbox, so that
             * we never nuke the mailbox from the replica before realising
             * that it has been renamed.  This can be moved later again when
             * we sync mailboxes by uniqueid rather than name... */
            sync_log_mailbox_double(oldname, newname);

            mailbox_rename_cleanup(&oldmailbox, isusermbox);

#ifdef WITH_DAV
            mailbox_add_dav(newmailbox);
#endif

            mailbox_close(&newmailbox);

            /* and log an append so that squatter indexes it */
            sync_log_append(newname);
        }
        else if (partitionmove) {
            char *oldpartition = xstrdup(oldmailbox->part);
            char *olduniqueid = xstrdup(oldmailbox->uniqueid);
            if (config_auditlog)
                syslog(LOG_NOTICE, "auditlog: partitionmove sessionid=<%s> "
                       "mailbox=<%s> uniqueid=<%s> oldpart=<%s> newpart=<%s>",
                       session_id(),
                       oldmailbox->name, oldmailbox->uniqueid,
                       oldpartition, partition);
            /* this will sync-log the name anyway */
            mailbox_close(&oldmailbox);
            mailbox_delete_cleanup(NULL, oldpartition, oldname, olduniqueid);
            free(olduniqueid);
            free(oldpartition);
        }
        else
            abort(); /* impossible, in theory */
    }

    /* free memory */
    free(newpartition);
    mboxlist_entry_free(&newmbentry);

    return r;
}
