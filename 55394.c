EXPORTED int mboxlist_lookup_allow_all(const char *name,
                                   mbentry_t **entryptr,
                                   struct txn **tid)
{
    return mboxlist_mylookup(name, entryptr, tid, 0);
}
