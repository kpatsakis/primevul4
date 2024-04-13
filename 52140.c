iperf_print_intermediate(struct iperf_test *test)
{
    char ubuf[UNIT_LEN];
    char nbuf[UNIT_LEN];
    struct iperf_stream *sp = NULL;
    struct iperf_interval_results *irp;
    iperf_size_t bytes = 0;
    double bandwidth;
    int retransmits = 0;
    double start_time, end_time;
    cJSON *json_interval;
    cJSON *json_interval_streams;
    int total_packets = 0, lost_packets = 0;
    double avg_jitter = 0.0, lost_percent;

    if (test->json_output) {
        json_interval = cJSON_CreateObject();
	if (json_interval == NULL)
	    return;
	cJSON_AddItemToArray(test->json_intervals, json_interval);
        json_interval_streams = cJSON_CreateArray();
	if (json_interval_streams == NULL)
	    return;
	cJSON_AddItemToObject(json_interval, "streams", json_interval_streams);
    } else {
        json_interval = NULL;
        json_interval_streams = NULL;
    }

    SLIST_FOREACH(sp, &test->streams, streams) {
        print_interval_results(test, sp, json_interval_streams);
	/* sum up all streams */
	irp = TAILQ_LAST(&sp->result->interval_results, irlisthead);
	if (irp == NULL) {
	    iperf_err(test, "iperf_print_intermediate error: interval_results is NULL");
	    return;
	}
        bytes += irp->bytes_transferred;
	if (test->protocol->id == Ptcp) {
	    if (test->sender && test->sender_has_retransmits) {
		retransmits += irp->interval_retrans;
	    }
	} else {
            total_packets += irp->interval_packet_count;
            lost_packets += irp->interval_cnt_error;
            avg_jitter += irp->jitter;
	}
    }

    /* next build string with sum of all streams */
    if (test->num_streams > 1 || test->json_output) {
        sp = SLIST_FIRST(&test->streams); /* reset back to 1st stream */
	/* Only do this of course if there was a first stream */
	if (sp) {
        irp = TAILQ_LAST(&sp->result->interval_results, irlisthead);    /* use 1st stream for timing info */

        unit_snprintf(ubuf, UNIT_LEN, (double) bytes, 'A');
	bandwidth = (double) bytes / (double) irp->interval_duration;
        unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);

        start_time = timeval_diff(&sp->result->start_time,&irp->interval_start_time);
        end_time = timeval_diff(&sp->result->start_time,&irp->interval_end_time);
	if (test->protocol->id == Ptcp) {
	    if (test->sender && test->sender_has_retransmits) {
		/* Interval sum, TCP with retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(json_interval, "sum", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  retransmits: %d  omitted: %b", (double) start_time, (double) end_time, (double) irp->interval_duration, (int64_t) bytes, bandwidth * 8, (int64_t) retransmits, irp->omitted)); /* XXX irp->omitted or test->omitting? */
		else
		    iprintf(test, report_sum_bw_retrans_format, start_time, end_time, ubuf, nbuf, retransmits, irp->omitted?report_omitted:""); /* XXX irp->omitted or test->omitting? */
	    } else {
		/* Interval sum, TCP without retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(json_interval, "sum", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  omitted: %b", (double) start_time, (double) end_time, (double) irp->interval_duration, (int64_t) bytes, bandwidth * 8, test->omitting));
		else
		    iprintf(test, report_sum_bw_format, start_time, end_time, ubuf, nbuf, test->omitting?report_omitted:"");
	    }
	} else {
	    /* Interval sum, UDP. */
	    if (test->sender) {
		if (test->json_output)
		    cJSON_AddItemToObject(json_interval, "sum", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  packets: %d  omitted: %b", (double) start_time, (double) end_time, (double) irp->interval_duration, (int64_t) bytes, bandwidth * 8, (int64_t) total_packets, test->omitting));
		else
		    iprintf(test, report_sum_bw_udp_sender_format, start_time, end_time, ubuf, nbuf, total_packets, test->omitting?report_omitted:"");
	    } else {
		avg_jitter /= test->num_streams;
		lost_percent = 100.0 * lost_packets / total_packets;
		if (test->json_output)
		    cJSON_AddItemToObject(json_interval, "sum", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  jitter_ms: %f  lost_packets: %d  packets: %d  lost_percent: %f  omitted: %b", (double) start_time, (double) end_time, (double) irp->interval_duration, (int64_t) bytes, bandwidth * 8, (double) avg_jitter * 1000.0, (int64_t) lost_packets, (int64_t) total_packets, (double) lost_percent, test->omitting));
		else
		    iprintf(test, report_sum_bw_udp_format, start_time, end_time, ubuf, nbuf, avg_jitter * 1000.0, lost_packets, total_packets, lost_percent, test->omitting?report_omitted:"");
	    }
	}
	}
    }
}
