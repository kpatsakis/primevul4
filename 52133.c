iperf_init_test(struct iperf_test *test)
{
    struct timeval now;
    struct iperf_stream *sp;

    if (test->protocol->init) {
        if (test->protocol->init(test) < 0)
            return -1;
    }

    /* Init each stream. */
    if (gettimeofday(&now, NULL) < 0) {
	i_errno = IEINITTEST;
	return -1;
    }
    SLIST_FOREACH(sp, &test->streams, streams) {
	sp->result->start_time = now;
    }

    if (test->on_test_start)
        test->on_test_start(test);

    return 0;
}
