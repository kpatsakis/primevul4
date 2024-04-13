static int allmbox_cb(void *rock,
                      const char *key,
                      size_t keylen,
                      const char *data,
                      size_t datalen)
{
    struct allmb_rock *mbrock = (struct allmb_rock *)rock;

    if (!mbrock->mbentry) {
        int r = mboxlist_parse_entry(&mbrock->mbentry, key, keylen, data, datalen);
        if (r) return r;
    }

    return mbrock->proc(mbrock->mbentry, mbrock->rock);
}
