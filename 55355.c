static int alluser_cb(const mbentry_t *mbentry, void *rock)
{
    struct alluser_rock *urock = (struct alluser_rock *)rock;
    char *userid = mboxname_to_userid(mbentry->name);
    int r = 0;

    if (userid) {
        if (strcmpsafe(urock->prev, userid)) {
            r = urock->proc(userid, urock->rock);
            free(urock->prev);
            urock->prev = xstrdup(userid);
        }
        free(userid);
    }

    return r;
}
