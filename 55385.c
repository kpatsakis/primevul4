EXPORTED int mboxlist_findallmulti(struct namespace *namespace,
                                   const strarray_t *patterns, int isadmin,
                                   const char *userid, const struct auth_state *auth_state,
                                   findall_cb *proc, void *rock)
{
    int r = 0;

    if (!namespace) namespace = mboxname_get_adminnamespace();

    struct find_rock cbrock;
    memset(&cbrock, 0, sizeof(struct find_rock));

    cbrock.auth_state = auth_state;
    cbrock.db = mbdb;
    cbrock.isadmin = isadmin;
    cbrock.namespace = namespace;
    cbrock.proc = proc;
    cbrock.procrock = rock;
    cbrock.userid = userid;
    if (userid) {
        const char *domp = strchr(userid, '@');
        if (domp) cbrock.domain = domp + 1;
    }

    r = mboxlist_do_find(&cbrock, patterns);

    return r;
}
