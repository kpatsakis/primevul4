EXPORTED mbentry_t *mboxlist_entry_copy(const mbentry_t *src)
{
    mbentry_t *copy = mboxlist_entry_create();
    copy->name = xstrdupnull(src->name);
    copy->ext_name = xstrdupnull(src->ext_name);

    copy->mtime = src->mtime;
    copy->uidvalidity = src->uidvalidity;
    copy->mbtype = src->mbtype;
    copy->foldermodseq = src->foldermodseq;

    copy->partition = xstrdupnull(src->partition);
    copy->server = xstrdupnull(src->server);
    copy->acl = xstrdupnull(src->acl);
    copy->uniqueid = xstrdupnull(src->uniqueid);

    copy->legacy_specialuse = xstrdupnull(src->legacy_specialuse);

    return copy;
}
