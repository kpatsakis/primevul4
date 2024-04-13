EXPORTED char *mbentry_metapath(const struct mboxlist_entry *mbentry, int metatype, int isnew)
{
    return mboxname_metapath(mbentry->partition,
                             mbentry->name,
                             mbentry->uniqueid,
                             metatype,
                             isnew);
}
