static int mboxlist_create_namecheck(const char *mboxname,
                                     const char *userid,
                                     const struct auth_state *auth_state,
                                     int isadmin, int force_subdirs)
{
    mbentry_t *mbentry = NULL;
    int r = 0;

    /* policy first */
    r = mboxname_policycheck(mboxname);
    if (r) goto done;

    /* is this the user's INBOX namespace? */
    if (!isadmin && mboxname_userownsmailbox(userid, mboxname)) {
        /* User has admin rights over their own mailbox namespace */
        if (config_implicitrights & ACL_ADMIN)
            isadmin = 1;
    }

    /* Check to see if mailbox already exists */
    r = mboxlist_lookup(mboxname, &mbentry, NULL);
    if (r != IMAP_MAILBOX_NONEXISTENT) {
        if (!r) {
            r = IMAP_MAILBOX_EXISTS;

            /* Lie about error if privacy demands */
            if (!isadmin &&
                !(cyrus_acl_myrights(auth_state, mbentry->acl) & ACL_LOOKUP)) {
                r = IMAP_PERMISSION_DENIED;
            }
        }

        goto done;
    }
    mboxlist_entry_free(&mbentry);

    /* look for a parent mailbox */
    r = mboxlist_findparent(mboxname, &mbentry);
    if (r == 0) {
        /* found a parent */
        char root[MAX_MAILBOX_NAME+1];

        /* check acl */
        if (!isadmin &&
            !(cyrus_acl_myrights(auth_state, mbentry->acl) & ACL_CREATE)) {
            r = IMAP_PERMISSION_DENIED;
            goto done;
        }

        /* check quota */
        if (quota_findroot(root, sizeof(root), mboxname)) {
            quota_t qdiffs[QUOTA_NUMRESOURCES] = QUOTA_DIFFS_DONTCARE_INITIALIZER;
            qdiffs[QUOTA_NUMFOLDERS] = 1;
            r = quota_check_useds(root, qdiffs);
            if (r) goto done;
        }
    }
    else if (r == IMAP_MAILBOX_NONEXISTENT) {
        /* no parent mailbox */
        if (!isadmin) {
            r = IMAP_PERMISSION_DENIED;
            goto done;
        }

        if (!force_subdirs) {
            mbname_t *mbname = mbname_from_intname(mboxname);
            if (!mbname_isdeleted(mbname) && mbname_userid(mbname) && strarray_size(mbname_boxes(mbname))) {
                /* Disallow creating user.X.* when no user.X */
                r = IMAP_PERMISSION_DENIED;
                goto done;
            }
            mbname_free(&mbname);
        }

        /* otherwise no parent is OK */
        r = 0;
    }

done:
    mboxlist_entry_free(&mbentry);

    return r;
}
