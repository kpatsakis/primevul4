static int find_p(void *rockp,
                  const char *key, size_t keylen,
                  const char *data, size_t datalen)
{
    struct find_rock *rock = (struct find_rock *) rockp;
    char intname[MAX_MAILBOX_PATH+1];
    int i;

    /* skip any $RACL or future $ space keys */
    if (key[0] == '$') return 0;

    memcpy(intname, key, keylen);
    intname[keylen] = 0;

    assert(!rock->mbname);
    rock->mbname = mbname_from_intname(intname);

    if (!rock->isadmin && !config_getswitch(IMAPOPT_CROSSDOMAINS)) {
        /* don't list mailboxes outside of the default domain */
        if (strcmpsafe(rock->domain, mbname_domain(rock->mbname)))
            goto nomatch;
    }

    if (rock->mb_category && mbname_category(rock->mbname, rock->namespace, rock->userid) != rock->mb_category)
        goto nomatch;

    /* NOTE: this will all be cleaned up to be much more efficient sooner or later, with
     * a mbname_t being kept inside the mbentry, and the extname cached all the way to
     * final use.  For now, we pay the cost of re-calculating for simplicity of the
     * changes to mbname_t itself */
    const char *extname = mbname_extname(rock->mbname, rock->namespace, rock->userid);
    if (!extname) goto nomatch;

    int matchlen = 0;
    for (i = 0; i < rock->globs.count; i++) {
        glob *g = ptrarray_nth(&rock->globs, i);
        int thismatch = glob_test(g, extname);
        if (thismatch > matchlen) matchlen = thismatch;
    }

    /* If its not a match, skip it -- partial matches are ok. */
    if (!matchlen) goto nomatch;

    rock->matchlen = matchlen;

    /* subs DB has empty keys */
    if (rock->issubs)
        goto good;

    /* ignore entirely deleted records */
    if (mboxlist_parse_entry(&rock->mbentry, key, keylen, data, datalen))
        goto nomatch;

    /* nobody sees tombstones */
    if (rock->mbentry->mbtype & MBTYPE_DELETED)
        goto nomatch;

    /* check acl */
    if (!rock->isadmin) {
        /* always suppress deleted for non-admin */
        if (mbname_isdeleted(rock->mbname)) goto nomatch;

        /* check the acls */
        if (!(cyrus_acl_myrights(rock->auth_state, rock->mbentry->acl) & ACL_LOOKUP)) goto nomatch;
    }

good:
    return 1;

nomatch:
    mboxlist_entry_free(&rock->mbentry);
    mbname_free(&rock->mbname);
    return 0;
}
