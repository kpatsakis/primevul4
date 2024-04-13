EXPORTED char *mbentry_datapath(const struct mboxlist_entry *mbentry, uint32_t uid)
{
    return mboxname_datapath(mbentry->partition,
                             mbentry->name,
                             mbentry->uniqueid,
                             uid);
}
