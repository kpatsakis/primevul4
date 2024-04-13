static int query_get_string_answer(cmd_request_t cmd)
{
	struct booth_site *site;
	struct boothc_hdr_msg reply;
	struct boothc_header *header;
	char *data;
	int data_len;
	int rv;
	struct booth_transport const *tpt;
	int (*test_reply_f) (cmd_result_t reply_code, cmd_request_t cmd);
	size_t msg_size;
	void *request;

	if (cl.type == GEOSTORE) {
		test_reply_f = test_attr_reply;
		msg_size = sizeof(cl.attr_msg);
		request = &cl.attr_msg;
	} else {
		test_reply_f = test_reply;
		msg_size = sizeof(cl.msg);
		request = &cl.msg;
	}
	header = (struct boothc_header *)request;
	data = NULL;

	init_header(header, cmd, 0, cl.options, 0, 0, msg_size);

	if (!*cl.site)
		site = local;
	else if (!find_site_by_name(cl.site, &site, 1)) {
		log_error("cannot find site \"%s\"", cl.site);
		rv = ENOENT;
		goto out;
	}

	tpt = booth_transport + TCP;
	rv = tpt->open(site);
	if (rv < 0)
		goto out_close;

	rv = tpt->send(site, request, msg_size);
	if (rv < 0)
		goto out_close;

	rv = tpt->recv_auth(site, &reply, sizeof(reply));
	if (rv < 0)
		goto out_close;

	data_len = ntohl(reply.header.length) - rv;

	/* no attribute, or no ticket found */
	if (!data_len) {
		goto out_test_reply;
	}

	data = malloc(data_len+1);
	if (!data) {
		rv = -ENOMEM;
		goto out_close;
	}
	rv = tpt->recv(site, data, data_len);
	if (rv < 0)
		goto out_close;
	*(data+data_len) = '\0';

	*(data + data_len) = '\0';
	(void)fputs(data, stdout);
	fflush(stdout);
	rv = 0;

out_test_reply:
	rv = test_reply_f(ntohl(reply.header.result), cmd);
out_close:
	tpt->close(site);
out:
	if (data)
		free(data);
	return rv;
}