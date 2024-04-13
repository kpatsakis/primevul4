static int mboxlist_update_racl(const char *name, const mbentry_t *oldmbentry, const mbentry_t *newmbentry, struct txn **txn)
{
    static strarray_t *admins = NULL;
    struct buf buf = BUF_INITIALIZER;
    char *userid = mboxname_to_userid(name);
    strarray_t *oldusers = NULL;
    strarray_t *newusers = NULL;
    int i;
    int r = 0;

    if (!admins) admins = strarray_split(config_getstring(IMAPOPT_ADMINS), NULL, 0);

    if (oldmbentry && oldmbentry->mbtype != MBTYPE_DELETED)
        oldusers = strarray_split(oldmbentry->acl, "\t", 0);

    if (newmbentry && newmbentry->mbtype != MBTYPE_DELETED)
        newusers = strarray_split(newmbentry->acl, "\t", 0);

    if (oldusers) {
        for (i = 0; i+1 < strarray_size(oldusers); i+=2) {
            const char *acluser = strarray_nth(oldusers, i);
            const char *aclval = strarray_nth(oldusers, i+1);
            if (!strchr(aclval, 'l')) continue; /* non-lookup ACLs can be skipped */
            if (!strcmpsafe(userid, acluser)) continue;
            if (strarray_find(admins, acluser, 0) >= 0) continue;
            if (user_is_in(newusers, acluser)) continue;
            mboxlist_racl_key(!!userid, acluser, name, &buf);
            r = cyrusdb_delete(mbdb, buf.s, buf.len, txn, /*force*/1);
            if (r) goto done;
        }
    }

    if (newusers) {
        for (i = 0; i+1 < strarray_size(newusers); i+=2) {
            const char *acluser = strarray_nth(newusers, i);
            const char *aclval = strarray_nth(newusers, i+1);
            if (!strchr(aclval, 'l')) continue; /* non-lookup ACLs can be skipped */
            if (!strcmpsafe(userid, acluser)) continue;
            if (strarray_find(admins, acluser, 0) >= 0) continue;
            if (user_is_in(oldusers, acluser)) continue;
            mboxlist_racl_key(!!userid, acluser, name, &buf);
            r = cyrusdb_store(mbdb, buf.s, buf.len, "", 0, txn);
            if (r) goto done;
        }
    }

 done:
    strarray_free(oldusers);
    strarray_free(newusers);
    free(userid);
    buf_free(&buf);
    return r;
}
