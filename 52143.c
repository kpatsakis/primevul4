iperf_reset_stats(struct iperf_test *test)
{
    struct timeval now;
    struct iperf_stream *sp;
    struct iperf_stream_result *rp;

    test->bytes_sent = 0;
    test->blocks_sent = 0;
    gettimeofday(&now, NULL);
    SLIST_FOREACH(sp, &test->streams, streams) {
	sp->omitted_packet_count = sp->packet_count;
	sp->jitter = 0;
	sp->outoforder_packets = 0;
	sp->cnt_error = 0;
	rp = sp->result;
        rp->bytes_sent = rp->bytes_received = 0;
        rp->bytes_sent_this_interval = rp->bytes_received_this_interval = 0;
	if (test->sender && test->sender_has_retransmits) {
	    struct iperf_interval_results ir; /* temporary results structure */
	    save_tcpinfo(sp, &ir);
	    rp->stream_prev_total_retrans = get_total_retransmits(&ir);
	}
	rp->stream_retrans = 0;
	rp->start_time = now;
    }
}
