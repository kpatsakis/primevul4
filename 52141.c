iperf_print_results(struct iperf_test *test)
{

    cJSON *json_summary_streams = NULL;
    cJSON *json_summary_stream = NULL;
    int total_retransmits = 0;
    int total_packets = 0, lost_packets = 0;
    char ubuf[UNIT_LEN];
    char nbuf[UNIT_LEN];
    struct stat sb;
    char sbuf[UNIT_LEN];
    struct iperf_stream *sp = NULL;
    iperf_size_t bytes_sent, total_sent = 0;
    iperf_size_t bytes_received, total_received = 0;
    double start_time, end_time, avg_jitter = 0.0, lost_percent;
    double bandwidth;

    /* print final summary for all intervals */

    if (test->json_output) {
        json_summary_streams = cJSON_CreateArray();
	if (json_summary_streams == NULL)
	    return;
	cJSON_AddItemToObject(test->json_end, "streams", json_summary_streams);
    } else {
	iprintf(test, "%s", report_bw_separator);
	if (test->verbose)
	    iprintf(test, "%s", report_summary);
	if (test->protocol->id == Ptcp) {
	    if (test->sender_has_retransmits)
		iprintf(test, "%s", report_bw_retrans_header);
	    else
		iprintf(test, "%s", report_bw_header);
	} else
	    iprintf(test, "%s", report_bw_udp_header);
    }

    start_time = 0.;
    sp = SLIST_FIRST(&test->streams);
    /* 
     * If there is at least one stream, then figure out the length of time
     * we were running the tests and print out some statistics about
     * the streams.  It's possible to not have any streams at all
     * if the client got interrupted before it got to do anything.
     */
    if (sp) {
    end_time = timeval_diff(&sp->result->start_time, &sp->result->end_time);
    SLIST_FOREACH(sp, &test->streams, streams) {
	if (test->json_output) {
	    json_summary_stream = cJSON_CreateObject();
	    if (json_summary_stream == NULL)
		return;
	    cJSON_AddItemToArray(json_summary_streams, json_summary_stream);
	}

        bytes_sent = sp->result->bytes_sent;
        bytes_received = sp->result->bytes_received;
        total_sent += bytes_sent;
        total_received += bytes_received;

        if (test->protocol->id == Ptcp) {
	    if (test->sender_has_retransmits) {
		total_retransmits += sp->result->stream_retrans;
	    }
	} else {
            total_packets += (sp->packet_count - sp->omitted_packet_count);
            lost_packets += sp->cnt_error;
            avg_jitter += sp->jitter;
        }

	unit_snprintf(ubuf, UNIT_LEN, (double) bytes_sent, 'A');
	bandwidth = (double) bytes_sent / (double) end_time;
	unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
	if (test->protocol->id == Ptcp) {
	    if (test->sender_has_retransmits) {
		/* Summary, TCP with retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(json_summary_stream, "sender", iperf_json_printf("socket: %d  start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  retransmits: %d", (int64_t) sp->socket, (double) start_time, (double) end_time, (double) end_time, (int64_t) bytes_sent, bandwidth * 8, (int64_t) sp->result->stream_retrans));
		else
		    iprintf(test, report_bw_retrans_format, sp->socket, start_time, end_time, ubuf, nbuf, sp->result->stream_retrans, report_sender);
	    } else {
		/* Summary, TCP without retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(json_summary_stream, "sender", iperf_json_printf("socket: %d  start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f", (int64_t) sp->socket, (double) start_time, (double) end_time, (double) end_time, (int64_t) bytes_sent, bandwidth * 8));
		else
		    iprintf(test, report_bw_format, sp->socket, start_time, end_time, ubuf, nbuf, report_sender);
	    }
	} else {
	    /* Summary, UDP. */
	    lost_percent = 100.0 * sp->cnt_error / (sp->packet_count - sp->omitted_packet_count);
	    if (test->json_output)
		cJSON_AddItemToObject(json_summary_stream, "udp", iperf_json_printf("socket: %d  start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  jitter_ms: %f  lost_packets: %d  packets: %d  lost_percent: %f  out_of_order: %d", (int64_t) sp->socket, (double) start_time, (double) end_time, (double) end_time, (int64_t) bytes_sent, bandwidth * 8, (double) sp->jitter * 1000.0, (int64_t) sp->cnt_error, (int64_t) (sp->packet_count - sp->omitted_packet_count), (double) lost_percent, (int64_t) sp->outoforder_packets));
	    else {
		iprintf(test, report_bw_udp_format, sp->socket, start_time, end_time, ubuf, nbuf, sp->jitter * 1000.0, sp->cnt_error, (sp->packet_count - sp->omitted_packet_count), lost_percent, "");
		if (test->role == 'c')
		    iprintf(test, report_datagrams, sp->socket, (sp->packet_count - sp->omitted_packet_count));
		if (sp->outoforder_packets > 0)
		    iprintf(test, report_sum_outoforder, start_time, end_time, sp->outoforder_packets);
	    }
	}

	if (sp->diskfile_fd >= 0) {
	    if (fstat(sp->diskfile_fd, &sb) == 0) {
		int percent = (int) ( ( (double) bytes_sent / (double) sb.st_size ) * 100.0 );
		unit_snprintf(sbuf, UNIT_LEN, (double) sb.st_size, 'A');
		if (test->json_output)
		    cJSON_AddItemToObject(json_summary_stream, "diskfile", iperf_json_printf("sent: %d  size: %d  percent: %d  filename: %s", (int64_t) bytes_sent, (int64_t) sb.st_size, (int64_t) percent, test->diskfile_name));
		else
		    iprintf(test, report_diskfile, ubuf, sbuf, percent, test->diskfile_name);
	    }
	}

	unit_snprintf(ubuf, UNIT_LEN, (double) bytes_received, 'A');
	bandwidth = (double) bytes_received / (double) end_time;
	unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
	if (test->protocol->id == Ptcp) {
	    if (test->json_output)
		cJSON_AddItemToObject(json_summary_stream, "receiver", iperf_json_printf("socket: %d  start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f", (int64_t) sp->socket, (double) start_time, (double) end_time, (double) end_time, (int64_t) bytes_received, bandwidth * 8));
	    else
		iprintf(test, report_bw_format, sp->socket, start_time, end_time, ubuf, nbuf, report_receiver);
	}
    }
    }

    if (test->num_streams > 1 || test->json_output) {
        unit_snprintf(ubuf, UNIT_LEN, (double) total_sent, 'A');
	/* If no tests were run, arbitrariliy set bandwidth to 0. */
	if (end_time > 0.0) {
	    bandwidth = (double) total_sent / (double) end_time;
	}
	else {
	    bandwidth = 0.0;
	}
        unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
        if (test->protocol->id == Ptcp) {
	    if (test->sender_has_retransmits) {
		/* Summary sum, TCP with retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(test->json_end, "sum_sent", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  retransmits: %d", (double) start_time, (double) end_time, (double) end_time, (int64_t) total_sent, bandwidth * 8, (int64_t) total_retransmits));
		else
		    iprintf(test, report_sum_bw_retrans_format, start_time, end_time, ubuf, nbuf, total_retransmits, report_sender);
	    } else {
		/* Summary sum, TCP without retransmits. */
		if (test->json_output)
		    cJSON_AddItemToObject(test->json_end, "sum_sent", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f", (double) start_time, (double) end_time, (double) end_time, (int64_t) total_sent, bandwidth * 8));
		else
		    iprintf(test, report_sum_bw_format, start_time, end_time, ubuf, nbuf, report_sender);
	    }
            unit_snprintf(ubuf, UNIT_LEN, (double) total_received, 'A');
	    /* If no tests were run, set received bandwidth to 0 */
	    if (end_time > 0.0) {
		bandwidth = (double) total_received / (double) end_time;
	    }
	    else {
		bandwidth = 0.0;
	    }
            unit_snprintf(nbuf, UNIT_LEN, bandwidth, test->settings->unit_format);
	    if (test->json_output)
		cJSON_AddItemToObject(test->json_end, "sum_received", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f", (double) start_time, (double) end_time, (double) end_time, (int64_t) total_received, bandwidth * 8));
	    else
		iprintf(test, report_sum_bw_format, start_time, end_time, ubuf, nbuf, report_receiver);
        } else {
	    /* Summary sum, UDP. */
            avg_jitter /= test->num_streams;
	    /* If no packets were sent, arbitrarily set loss percentage to 100. */
	    if (total_packets > 0) {
		lost_percent = 100.0 * lost_packets / total_packets;
	    }
	    else {
		lost_percent = 100.0;
	    }
	    if (test->json_output)
		cJSON_AddItemToObject(test->json_end, "sum", iperf_json_printf("start: %f  end: %f  seconds: %f  bytes: %d  bits_per_second: %f  jitter_ms: %f  lost_packets: %d  packets: %d  lost_percent: %f", (double) start_time, (double) end_time, (double) end_time, (int64_t) total_sent, bandwidth * 8, (double) avg_jitter * 1000.0, (int64_t) lost_packets, (int64_t) total_packets, (double) lost_percent));
	    else
		iprintf(test, report_sum_bw_udp_format, start_time, end_time, ubuf, nbuf, avg_jitter * 1000.0, lost_packets, total_packets, lost_percent, "");
        }
    }

    if (test->json_output)
	cJSON_AddItemToObject(test->json_end, "cpu_utilization_percent", iperf_json_printf("host_total: %f  host_user: %f  host_system: %f  remote_total: %f  remote_user: %f  remote_system: %f", (double) test->cpu_util[0], (double) test->cpu_util[1], (double) test->cpu_util[2], (double) test->remote_cpu_util[0], (double) test->remote_cpu_util[1], (double) test->remote_cpu_util[2]));
    else {
	if (test->verbose) {
	    iprintf(test, report_cpu, report_local, test->sender?report_sender:report_receiver, test->cpu_util[0], test->cpu_util[1], test->cpu_util[2], report_remote, test->sender?report_receiver:report_sender, test->remote_cpu_util[0], test->remote_cpu_util[1], test->remote_cpu_util[2]);
	}

	/* Print server output if we're on the client and it was requested/provided */
	if (test->role == 'c' && iperf_get_test_get_server_output(test)) {
	    if (test->json_server_output) {
		iprintf(test, "\nServer JSON output:\n%s\n", cJSON_Print(test->json_server_output));
		cJSON_Delete(test->json_server_output);
		test->json_server_output = NULL;
	    }
	    if (test->server_output_text) {
		iprintf(test, "\nServer output:\n%s\n", test->server_output_text);
		test->server_output_text = NULL;
	    }
	}
    }
}
