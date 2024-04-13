EXPORTED void mboxlist_entry_free(mbentry_t **mbentryptr)
{
    mbentry_t *mbentry = *mbentryptr;

    /* idempotent */
    if (!mbentry) return;

    free(mbentry->name);
    free(mbentry->ext_name);

    free(mbentry->partition);
    free(mbentry->server);
    free(mbentry->acl);
    free(mbentry->uniqueid);

    free(mbentry->legacy_specialuse);

    free(mbentry);

    *mbentryptr = NULL;
}
