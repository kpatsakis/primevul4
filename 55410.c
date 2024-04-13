EXPORTED int mboxlist_unsetquota(const char *root)
{
    struct quota q;
    int r=0;

    if (!root[0] || root[0] == '.' || strchr(root, '/')
        || strchr(root, '*') || strchr(root, '%') || strchr(root, '?')) {
        return IMAP_MAILBOX_BADNAME;
    }

    quota_init(&q, root);
    r = quota_read(&q, NULL, 0);
    /* already unset */
    if (r == IMAP_QUOTAROOT_NONEXISTENT) {
        r = 0;
        goto done;
    }
    if (r) goto done;

    r = quota_changelock();

    /*
     * Have to remove it from all affected mailboxes
     */
    mboxlist_mboxtree(root, mboxlist_rmquota, (void *)root, /*flags*/0);

    r = quota_deleteroot(root);
    quota_changelockrelease();

    if (!r) sync_log_quota(root);

 done:
    quota_free(&q);
    return r;
}
