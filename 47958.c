local void in_init(void)
{
    g.in_left = 0;
    g.in_eof = 0;
    g.in_short = 0;
    g.in_tot = 0;
#ifndef NOTHREAD
    g.in_which = -1;
#endif
}
