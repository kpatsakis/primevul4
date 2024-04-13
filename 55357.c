static int find_cb(void *rockp,
                   /* XXX - confirm these are the same?  - nah */
                   const char *key __attribute__((unused)),
                   size_t keylen __attribute__((unused)),
                   const char *data __attribute__((unused)),
                   size_t datalen __attribute__((unused)))
{
    struct find_rock *rock = (struct find_rock *) rockp;
    char *testname = NULL;
    int r = 0;
    int i;

    if (rock->checkmboxlist && !rock->mbentry) {
        r = mboxlist_lookup(mbname_intname(rock->mbname), &rock->mbentry, NULL);
        if (r) {
            if (r == IMAP_MAILBOX_NONEXISTENT) r = 0;
            goto done;
        }
    }

    const char *extname = mbname_extname(rock->mbname, rock->namespace, rock->userid);
    testname = xstrndup(extname, rock->matchlen);

    struct findall_data fdata = { testname, rock->mb_category, rock->mbentry, NULL };

    if (rock->singlepercent) {
        char sep = rock->namespace->hier_sep;
        char *p = testname;
        /* we need to try all the previous names in order */
        while ((p = strchr(p, sep)) != NULL) {
            *p = '\0';

            /* only if this expression could fully match */
            int matchlen = 0;
            for (i = 0; i < rock->globs.count; i++) {
                glob *g = ptrarray_nth(&rock->globs, i);
                int thismatch = glob_test(g, testname);
                if (thismatch > matchlen) matchlen = thismatch;
            }

            if (matchlen == (int)strlen(testname)) {
                r = (*rock->proc)(&fdata, rock->procrock);
                if (r) goto done;
            }

            /* replace the separator for the next longest name */
            *p++ = sep;
        }
    }

    /* mbname confirms that it's an exact match */
    if (rock->matchlen == (int)strlen(extname))
        fdata.mbname = rock->mbname;

    r = (*rock->proc)(&fdata, rock->procrock);

 done:
    free(testname);
    mboxlist_entry_free(&rock->mbentry);
    mbname_free(&rock->mbname);
    return r;
}
