EXPORTED int mboxlist_findone(struct namespace *namespace,
                              const char *intname, int isadmin,
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

    mbname_t *mbname = mbname_from_intname(intname);
    glob *g = glob_init(mbname_extname(mbname, namespace, userid),
                        namespace->hier_sep);
    ptrarray_append(&cbrock.globs, g);
    mbname_free(&mbname);

    r = cyrusdb_forone(cbrock.db, intname, strlen(intname), &find_p, &find_cb, &cbrock, NULL);

    glob_free(&g);
    ptrarray_fini(&cbrock.globs);

    return r;
}
