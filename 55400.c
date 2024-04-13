EXPORTED int mboxlist_parse_entry(mbentry_t **mbentryptr,
                                  const char *name, size_t namelen,
                                  const char *data, size_t datalen)
{
    static struct buf aclbuf;
    int r = IMAP_MAILBOX_BADFORMAT;
    char *freeme = NULL;
    char **target;
    char *p, *q;
    mbentry_t *mbentry = mboxlist_entry_create();

    if (!datalen)
        goto done;

    /* copy name */
    if (namelen)
        mbentry->name = xstrndup(name, namelen);
    else
        mbentry->name = xstrdup(name);

    /* check for DLIST mboxlist */
    if (*data == '%') {
        struct parseentry_rock rock;
        memset(&rock, 0, sizeof(struct parseentry_rock));
        rock.mbentry = mbentry;
        rock.aclbuf = &aclbuf;
        aclbuf.len = 0;
        r = dlist_parsesax(data, datalen, 0, parseentry_cb, &rock);
        if (!r) mbentry->acl = buf_newcstring(&aclbuf);
        goto done;
    }

    /* copy data */
    freeme = p = xstrndup(data, datalen);

    /* check for extended mboxlist entry */
    if (*p == '(') {
        int last = 0;
        p++; /* past leading '(' */
        while (!last) {
            target = NULL;
            q = p;
            while (*q && *q != ' ' && *q != ')') q++;
            if (*q != ' ') break;
            *q++ = '\0';
            if (!strcmp(p, "uniqueid")) target = &mbentry->uniqueid;
            if (!strcmp(p, "specialuse")) target = &mbentry->legacy_specialuse;
            p = q;
            while (*q && *q != ' ' && *q != ')') q++;
            if (*q != ' ') last = 1;
            if (*q) *q++ = '\0';
            if (target) *target = xstrdup(p);
            p = q;
        }
        if (*p == ' ') p++; /* past trailing ' ' */
    }

    /* copy out interesting parts */
    mbentry->mbtype = strtol(p, &p, 10);
    if (*p == ' ') p++;

    q = p;
    while (*q && *q != ' ' && *q != '!') q++;
    if (*q == '!') {
        *q++ = '\0';
        mbentry->server = xstrdup(p);
        p = q;
        while (*q && *q != ' ') q++;
    }
    if (*q) *q++ = '\0';
    mbentry->partition = xstrdup(p);

    mbentry->acl = xstrdup(q);

    r = 0;

done:
    if (!r && mbentryptr)
        *mbentryptr = mbentry;
    else mboxlist_entry_free(&mbentry);
    free(freeme);
    return r;
}
