local void cut_short(int sig)
{
    (void)sig;
    Trace(("termination by user"));
    if (g.outd != -1 && g.outf != NULL)
        unlink(g.outf);
    log_dump();
    _exit(1);
}
