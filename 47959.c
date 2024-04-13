local unsigned inb(void *desc, unsigned char **buf)
{
    (void)desc;
    load();
    *buf = g.in_next;
    return g.in_left;
}
