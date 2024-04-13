local void outb_write(void *dummy)
{
    size_t len;

    (void)dummy;

    Trace(("-- launched decompress write thread"));
    do {
        possess(outb_write_more);
        wait_for(outb_write_more, TO_BE, 1);
        len = out_len;
        if (len && g.decode == 1)
            writen(g.outd, out_copy, len);
        Trace(("-- decompress wrote %lu bytes", len));
        twist(outb_write_more, TO, 0);
    } while (len);
    Trace(("-- exited decompress write thread"));
}
