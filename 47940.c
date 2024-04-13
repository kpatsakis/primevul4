local void cat(void)
{
    /* write first magic byte (if we're here, there's at least one byte) */
    writen(g.outd, &g.magic1, 1);
    g.out_tot = 1;

    /* copy the remainder of the input to the output (if there were any more
       bytes of input, then g.in_left is non-zero and g.in_next is pointing to
       the second magic byte) */
    while (g.in_left) {
        writen(g.outd, g.in_next, g.in_left);
        g.out_tot += g.in_left;
        g.in_left = 0;
        load();
    }
}
