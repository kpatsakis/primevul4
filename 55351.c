static int _rename_check_specialuse(const char *oldname, const char *newname)
{
    mbname_t *old = mbname_from_intname(oldname);
    mbname_t *new = mbname_from_intname(newname);
    struct buf attrib = BUF_INITIALIZER;
    int r = 0;
    if (mbname_userid(old))
        annotatemore_lookup(oldname, "/specialuse", mbname_userid(old), &attrib);
    /* we have specialuse? */
    if (attrib.len) {
        /* then target must be a single-depth mailbox too */
        if (strarray_size(mbname_boxes(new)) != 1)
            r = IMAP_MAILBOX_SPECIALUSE;
        /* and have a userid as well */
        if (!mbname_userid(new))
            r = IMAP_MAILBOX_SPECIALUSE;
        /* and not be deleted */
        if (mbname_isdeleted(new))
            r = IMAP_MAILBOX_SPECIALUSE;
    }
    mbname_free(&new);
    mbname_free(&old);
    buf_free(&attrib);
    return r;
}
