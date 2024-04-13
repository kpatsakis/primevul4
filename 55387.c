EXPORTED int mboxlist_findparent(const char *mboxname,
                               mbentry_t **mbentryp)
{
    mbentry_t *mbentry = NULL;
    mbname_t *mbname = mbname_from_intname(mboxname);
    int r = IMAP_MAILBOX_NONEXISTENT;

    while (strarray_size(mbname_boxes(mbname))) {
        free(mbname_pop_boxes(mbname));
        mboxlist_entry_free(&mbentry);
        r = mboxlist_lookup(mbname_intname(mbname), &mbentry, NULL);
        if (r != IMAP_MAILBOX_NONEXISTENT)
            break;
    }

    if (r)
        mboxlist_entry_free(&mbentry);
    else
        *mbentryp = mbentry;

    mbname_free(&mbname);

    return r;
}
