EXPORTED int mboxlist_findsub(struct namespace *namespace,
                              const char *pattern, int isadmin,
                              const char *userid, const struct auth_state *auth_state,
                              findall_cb *proc, void *rock,
                              int force)
{
    strarray_t patterns = STRARRAY_INITIALIZER;
    strarray_append(&patterns, pattern);

    int r = mboxlist_findsubmulti(namespace, &patterns, isadmin, userid, auth_state, proc, rock, force);

    strarray_fini(&patterns);

    return r;
}
