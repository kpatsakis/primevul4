EXPORTED const char *mboxlist_mbtype_to_string(uint32_t mbtype)
{
    static struct buf buf = BUF_INITIALIZER;

    buf_reset(&buf);

    if (mbtype & MBTYPE_DELETED)
        buf_putc(&buf, 'd');
    if (mbtype & MBTYPE_MOVING)
        buf_putc(&buf, 'm');
    if (mbtype & MBTYPE_NETNEWS)
        buf_putc(&buf, 'n');
    if (mbtype & MBTYPE_REMOTE)
        buf_putc(&buf, 'r');
    if (mbtype & MBTYPE_RESERVE)
        buf_putc(&buf, 'z');
    if (mbtype & MBTYPE_CALENDAR)
        buf_putc(&buf, 'c');
    if (mbtype & MBTYPE_COLLECTION)
        buf_putc(&buf, 'b');
    if (mbtype & MBTYPE_ADDRESSBOOK)
        buf_putc(&buf, 'a');

    return buf_cstring(&buf);
}
