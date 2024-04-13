EXPORTED modseq_t mboxlist_foldermodseq_dirty(struct mailbox *mailbox)
{
    mbentry_t *mbentry = NULL;
    modseq_t ret = 0;

    if (mboxlist_mylookup(mailbox->name, &mbentry, NULL, 0))
        return 0;

    ret = mbentry->foldermodseq = mailbox_modseq_dirty(mailbox);

    mboxlist_update(mbentry, 0);

    mboxlist_entry_free(&mbentry);

    return ret;
}
