static char *mboxlist_entry_cstring(const mbentry_t *mbentry)
{
    struct buf buf = BUF_INITIALIZER;
    struct dlist *dl = dlist_newkvlist(NULL, mbentry->name);

    if (mbentry->acl)
        _write_acl(dl, mbentry->acl);

    if (mbentry->uniqueid)
        dlist_setatom(dl, "I", mbentry->uniqueid);

    if (mbentry->partition)
        dlist_setatom(dl, "P", mbentry->partition);

    if (mbentry->server)
        dlist_setatom(dl, "S", mbentry->server);

    if (mbentry->mbtype)
        dlist_setatom(dl, "T", mboxlist_mbtype_to_string(mbentry->mbtype));

    if (mbentry->uidvalidity)
        dlist_setnum32(dl, "V", mbentry->uidvalidity);

    if (mbentry->foldermodseq)
        dlist_setnum64(dl, "F", mbentry->foldermodseq);

    dlist_setdate(dl, "M", time(NULL));

    dlist_printbuf(dl, 0, &buf);

    dlist_free(&dl);

    return buf_release(&buf);
}
