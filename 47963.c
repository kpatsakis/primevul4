local void load_read(void *dummy)
{
    size_t len;

    (void)dummy;

    Trace(("-- launched decompress read thread"));
    do {
        possess(g.load_state);
        wait_for(g.load_state, TO_BE, 1);
        g.in_len = len = readn(g.ind, g.in_which ? g.in_buf : g.in_buf2, BUF);
        Trace(("-- decompress read thread read %lu bytes", len));
        twist(g.load_state, TO, 0);
    } while (len == BUF);
    Trace(("-- exited decompress read thread"));
}
