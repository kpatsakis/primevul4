static int do_command(cmd_request_t cmd)
{
	struct booth_site *site;
	struct boothc_ticket_msg reply;
	struct booth_transport const *tpt;
	uint32_t leader_id;
	int rv;
	int reply_cnt = 0, msg_logged = 0;
	const char *op_str = "";

	if (cmd == CMD_GRANT)
		op_str = "grant";
	else if (cmd == CMD_REVOKE)
		op_str = "revoke";

	rv = 0;
	site = NULL;

	/* Always use TCP for client - at least for now. */
	tpt = booth_transport + TCP;

	if (!*cl.site)
		site = local;
	else {
		if (!find_site_by_name(cl.site, &site, 1)) {
			log_error("Site \"%s\" not configured.", cl.site);
			goto out_close;
		}
	}

	if (site->type == ARBITRATOR) {
		if (site == local) {
			log_error("We're just an arbitrator, cannot grant/revoke tickets here.");
		} else {
			log_error("%s is just an arbitrator, cannot grant/revoke tickets there.", cl.site);
		}
		goto out_close;
	}

	assert(site->type == SITE);

	/* We don't check for existence of ticket, so that asking can be
	 * done without local configuration, too.
	 * Although, that means that the UDP port has to be specified, too. */
	if (!cl.msg.ticket.id[0]) {
		/* If the loaded configuration has only a single ticket defined, use that. */
		if (booth_conf->ticket_count == 1) {
			strncpy(cl.msg.ticket.id, booth_conf->ticket[0].name,
				sizeof(cl.msg.ticket.id));
		} else {
			log_error("No ticket given.");
			goto out_close;
		}
	}

redirect:
	init_header(&cl.msg.header, cmd, 0, cl.options, 0, 0, sizeof(cl.msg));

	rv = tpt->open(site);
	if (rv < 0)
		goto out_close;

	rv = tpt->send(site, &cl.msg, sendmsglen(&cl.msg));
	if (rv < 0)
		goto out_close;

read_more:
	rv = tpt->recv_auth(site, &reply, sizeof(reply));
	if (rv < 0) {
		/* print any errors depending on the code sent by the
		 * server */
		(void)test_reply(ntohl(reply.header.result), cmd);
		goto out_close;
	}

	rv = test_reply(ntohl(reply.header.result), cmd);
	if (rv == 1) {
		tpt->close(site);
		leader_id = ntohl(reply.ticket.leader);
		if (!find_site_by_id(leader_id, &site)) {
			log_error("Message with unknown redirect site %x received", leader_id);
			rv = -1;
			goto out_close;
		}
		goto redirect;
	} else if (rv == 2 || rv == 3) {
		/* the server has more to say */
		/* don't wait too long */
		if (reply_cnt > 1 && !(cl.options & OPT_WAIT)) {
			rv = 0;
			log_info("Giving up on waiting for the definite result. "
				 "Please use \"booth list\" later to "
				 "see the outcome.");
			goto out_close;
		}
		if (reply_cnt == 0) {
			log_info("%s request sent, "
				"waiting for the result ...", op_str);
			msg_logged++;
		} else if (rv == 3 && msg_logged < 2) {
			log_info("waiting for the CIB commit ...");
			msg_logged++;
		}
		reply_cnt++;
		goto read_more;
	}

out_close:
	if (site)
		tpt->close(site);
	return rv;
}