EXPORTED int mboxlist_ensureOwnerRights(void *rock, const char *identifier,
                               int myrights)
{
    char *owner = (char *)rock;
    if (strcmp(identifier, owner) != 0) return myrights;
    return myrights|config_implicitrights;
}
