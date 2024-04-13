EXPORTED strarray_t *mboxlist_sublist(const char *userid)
{
    struct db *subs = NULL;
    strarray_t *list = strarray_new();
    int r;

    /* open subs DB */
    r = mboxlist_opensubs(userid, &subs);
    if (r) goto done;

    /* faster to do it all in a single slurp! */
    r = cyrusdb_foreach(subs, "", 0, subsadd_cb, NULL, list, 0);

    mboxlist_closesubs(subs);

done:
    if (r) {
        strarray_free(list);
        return NULL;
    }
    return list;
}
