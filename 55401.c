static void mboxlist_racl_key(int isuser, const char *keyuser, const char *mbname, struct buf *buf)
{
    buf_setcstr(buf, "$RACL$");
    buf_putc(buf, isuser ? 'U' : 'S');
    buf_putc(buf, '$');
    if (keyuser) {
        buf_appendcstr(buf, keyuser);
        buf_putc(buf, '$');
    }
    if (mbname) {
        buf_appendcstr(buf, mbname);
    }
}
