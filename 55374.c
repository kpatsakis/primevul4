EXPORTED int mboxlist_delete(const char *name)
{
    return mboxlist_update_entry(name, NULL, NULL);
}
