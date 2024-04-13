local int bail(char *why, char *what)
{
    if (g.outd != -1 && g.outf != NULL)
        unlink(g.outf);
    complain("abort: %s%s", why, what);
    exit(1);
    return 0;
}
