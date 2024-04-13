EXPORTED int mboxlist_createmailboxcheck(const char *name, int mbtype __attribute__((unused)),
                                const char *partition,
                                int isadmin, const char *userid,
                                const struct auth_state *auth_state,
                                char **newacl, char **newpartition,
                                int forceuser)
{
    char *part = NULL;
    char *acl = NULL;
    int r = 0;

    r = mboxlist_create_namecheck(name, userid, auth_state,
                                  isadmin, forceuser);
    if (r) goto done;

    if (newacl) {
        r = mboxlist_create_acl(name, &acl);
        if (r) goto done;
    }

    if (newpartition) {
        r = mboxlist_create_partition(name, partition, &part);
        if (r) goto done;
    }

 done:
    if (r || !newacl) free(acl);
    else *newacl = acl;

    if (r || !newpartition) free(part);
    else *newpartition = part;

    return r;
}
