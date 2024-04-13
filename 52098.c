get_parameters(struct iperf_test *test)
{
    int r = 0;
    cJSON *j;
    cJSON *j_p;

    j = JSON_read(test->ctrl_sck);
    if (j == NULL) {
	i_errno = IERECVPARAMS;
        r = -1;
    } else {
	if (test->debug) {
	    printf("get_parameters:\n%s\n", cJSON_Print(j));
	}

	if ((j_p = cJSON_GetObjectItem(j, "tcp")) != NULL)
	    set_protocol(test, Ptcp);
	if ((j_p = cJSON_GetObjectItem(j, "udp")) != NULL)
	    set_protocol(test, Pudp);
	if ((j_p = cJSON_GetObjectItem(j, "omit")) != NULL)
	    test->omit = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "server_affinity")) != NULL)
	    test->server_affinity = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "time")) != NULL)
	    test->duration = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "num")) != NULL)
	    test->settings->bytes = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "blockcount")) != NULL)
	    test->settings->blocks = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "MSS")) != NULL)
	    test->settings->mss = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "nodelay")) != NULL)
	    test->no_delay = 1;
	if ((j_p = cJSON_GetObjectItem(j, "parallel")) != NULL)
	    test->num_streams = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "reverse")) != NULL)
	    iperf_set_test_reverse(test, 1);
	if ((j_p = cJSON_GetObjectItem(j, "window")) != NULL)
	    test->settings->socket_bufsize = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "len")) != NULL)
	    test->settings->blksize = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "bandwidth")) != NULL)
	    test->settings->rate = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "burst")) != NULL)
	    test->settings->burst = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "TOS")) != NULL)
	    test->settings->tos = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "flowlabel")) != NULL)
	    test->settings->flowlabel = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "title")) != NULL)
	    test->title = strdup(j_p->valuestring);
	if ((j_p = cJSON_GetObjectItem(j, "congestion")) != NULL)
	    test->congestion = strdup(j_p->valuestring);
	if ((j_p = cJSON_GetObjectItem(j, "get_server_output")) != NULL)
	    iperf_set_test_get_server_output(test, 1);
	if (test->sender && test->protocol->id == Ptcp && has_tcpinfo_retransmits())
	    test->sender_has_retransmits = 1;
	cJSON_Delete(j);
    }
    return r;
}
