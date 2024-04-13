local size_t load(void)
{
    /* if already detected end of file, do nothing */
    if (g.in_short) {
        g.in_eof = 1;
        g.in_left = 0;
        return 0;
    }

#ifndef NOTHREAD
    /* if first time in or procs == 1, read a buffer to have something to
       return, otherwise wait for the previous read job to complete */
    if (g.procs > 1) {
        /* if first time, fire up the read thread, ask for a read */
        if (g.in_which == -1) {
            g.in_which = 1;
            g.load_state = new_lock(1);
            g.load_thread = launch(load_read, NULL);
        }

        /* wait for the previously requested read to complete */
        possess(g.load_state);
        wait_for(g.load_state, TO_BE, 0);
        release(g.load_state);

        /* set up input buffer with the data just read */
        g.in_next = g.in_which ? g.in_buf : g.in_buf2;
        g.in_left = g.in_len;

        /* if not at end of file, alert read thread to load next buffer,
           alternate between g.in_buf and g.in_buf2 */
        if (g.in_len == BUF) {
            g.in_which = 1 - g.in_which;
            possess(g.load_state);
            twist(g.load_state, TO, 1);
        }

        /* at end of file -- join read thread (already exited), clean up */
        else {
            join(g.load_thread);
            free_lock(g.load_state);
            g.in_which = -1;
        }
    }
    else
#endif
    {
        /* don't use threads -- simply read a buffer into g.in_buf */
        g.in_left = readn(g.ind, g.in_next = g.in_buf, BUF);
    }

    /* note end of file */
    if (g.in_left < BUF) {
        g.in_short = 1;

        /* if we got bupkis, now is the time to mark eof */
        if (g.in_left == 0)
            g.in_eof = 1;
    }

    /* update the total and return the available bytes */
    g.in_tot += g.in_left;
    return g.in_left;
}
