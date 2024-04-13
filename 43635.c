sctp_disposition_t sctp_sf_t1_cookie_timer_expire(struct net *net,
					   const struct sctp_endpoint *ep,
					   const struct sctp_association *asoc,
					   const sctp_subtype_t type,
					   void *arg,
					   sctp_cmd_seq_t *commands)
{
	struct sctp_chunk *repl = NULL;
	int attempts = asoc->init_err_counter + 1;

	pr_debug("%s: timer T1 expired (COOKIE-ECHO)\n", __func__);

	SCTP_INC_STATS(net, SCTP_MIB_T1_COOKIE_EXPIREDS);

	if (attempts <= asoc->max_init_attempts) {
		repl = sctp_make_cookie_echo(asoc, NULL);
		if (!repl)
			return SCTP_DISPOSITION_NOMEM;

		sctp_add_cmd_sf(commands, SCTP_CMD_INIT_CHOOSE_TRANSPORT,
				SCTP_CHUNK(repl));
		/* Issue a sideeffect to do the needed accounting. */
		sctp_add_cmd_sf(commands, SCTP_CMD_COOKIEECHO_RESTART,
				SCTP_TO(SCTP_EVENT_TIMEOUT_T1_COOKIE));

		sctp_add_cmd_sf(commands, SCTP_CMD_REPLY, SCTP_CHUNK(repl));
	} else {
		sctp_add_cmd_sf(commands, SCTP_CMD_SET_SK_ERR,
				SCTP_ERROR(ETIMEDOUT));
		sctp_add_cmd_sf(commands, SCTP_CMD_INIT_FAILED,
				SCTP_PERR(SCTP_ERROR_NO_ERROR));
		return SCTP_DISPOSITION_DELETE_TCB;
	}

	return SCTP_DISPOSITION_CONSUME;
}
