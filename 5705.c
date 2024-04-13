static int test_reply(cmd_result_t reply_code, cmd_request_t cmd)
{
	int rv = 0;
	const char *op_str = "";

	if (cmd == CMD_GRANT)
		op_str = "grant";
	else if (cmd == CMD_REVOKE)
		op_str = "revoke";
	else if (cmd == CMD_LIST)
		op_str = "list";
	else if (cmd == CMD_PEERS)
		op_str = "peers";
	else {
		log_error("internal error reading reply result!");
		return -1;
	}

	switch (reply_code) {
	case RLT_OVERGRANT:
		log_info("You're granting a granted ticket. "
			 "If you wanted to migrate a ticket, "
			 "use revoke first, then use grant.");
		rv = -1;
		break;

	case RLT_TICKET_IDLE:
		log_info("ticket is not owned");
		rv = 0;
		break;

	case RLT_ASYNC:
		log_info("%s command sent, result will be returned "
			 "asynchronously. Please use \"booth list\" to "
			 "see the outcome.", op_str);
		rv = 0;
		break;

	case RLT_CIB_PENDING:
		log_info("%s succeeded (CIB commit pending)", op_str);
		/* wait for the CIB commit? */
		rv = (cl.options & OPT_WAIT_COMMIT) ? 3 : 0;
		break;

	case RLT_MORE:
		rv = 2;
		break;

	case RLT_SYNC_SUCC:
	case RLT_SUCCESS:
		if (cmd != CMD_LIST && cmd != CMD_PEERS)
			log_info("%s succeeded!", op_str);
		rv = 0;
		break;

	case RLT_SYNC_FAIL:
		log_info("%s failed!", op_str);
		rv = -1;
		break;

	case RLT_INVALID_ARG:
		log_error("ticket \"%s\" does not exist",
				cl.msg.ticket.id);
		rv = -1;
		break;

	case RLT_AUTH:
		log_error("authentication error");
		rv = -1;
		break;

	case RLT_EXT_FAILED:
		log_error("before-acquire-handler for ticket \"%s\" failed, grant denied",
				cl.msg.ticket.id);
		rv = -1;
		break;

	case RLT_ATTR_PREREQ:
		log_error("attr-prereq for ticket \"%s\" failed, grant denied",
				cl.msg.ticket.id);
		rv = -1;
		break;

	case RLT_REDIRECT:
		/* talk to another site */
		rv = 1;
		break;

	default:
		log_error("got an error code: %x", rv);
		rv = -1;
	}
	return rv;
}