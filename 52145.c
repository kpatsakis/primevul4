iperf_send(struct iperf_test *test, fd_set *write_setP)
{
    register int multisend, r, streams_active;
    register struct iperf_stream *sp;
    struct timeval now;

    /* Can we do multisend mode? */
    if (test->settings->burst != 0)
        multisend = test->settings->burst;
    else if (test->settings->rate == 0)
        multisend = test->multisend;
    else
        multisend = 1;	/* nope */

    for (; multisend > 0; --multisend) {
	if (test->settings->rate != 0 && test->settings->burst == 0)
	    gettimeofday(&now, NULL);
	streams_active = 0;
	SLIST_FOREACH(sp, &test->streams, streams) {
	    if (sp->green_light &&
	        (write_setP == NULL || FD_ISSET(sp->socket, write_setP))) {
		if ((r = sp->snd(sp)) < 0) {
		    if (r == NET_SOFTERROR)
			break;
		    i_errno = IESTREAMWRITE;
		    return r;
		}
		streams_active = 1;
		test->bytes_sent += r;
		++test->blocks_sent;
		if (test->settings->rate != 0 && test->settings->burst == 0)
		    iperf_check_throttle(sp, &now);
		if (multisend > 1 && test->settings->bytes != 0 && test->bytes_sent >= test->settings->bytes)
		    break;
		if (multisend > 1 && test->settings->blocks != 0 && test->blocks_sent >= test->settings->blocks)
		    break;
	    }
	}
	if (!streams_active)
	    break;
    }
    if (test->settings->burst != 0) {
	gettimeofday(&now, NULL);
	SLIST_FOREACH(sp, &test->streams, streams)
	    iperf_check_throttle(sp, &now);
    }
    if (write_setP != NULL)
	SLIST_FOREACH(sp, &test->streams, streams)
	    if (FD_ISSET(sp->socket, write_setP))
		FD_CLR(sp->socket, write_setP);

    return 0;
}
