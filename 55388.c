HIDDEN int mboxlist_findstage(const char *name, char *stagedir, size_t sd_len)
{
    const char *root;
    mbentry_t *mbentry = NULL;
    int r;

    assert(stagedir != NULL);

    /* Find mailbox */
    r = mboxlist_lookup(name, &mbentry, NULL);
    if (r) return r;

    root = config_partitiondir(mbentry->partition);
    mboxlist_entry_free(&mbentry);

    if (!root) return IMAP_PARTITION_UNKNOWN;

    snprintf(stagedir, sd_len, "%s/stage./", root);

    return 0;
}
