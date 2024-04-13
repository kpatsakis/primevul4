static int do_client(void)
{
	int rv;

	rv = setup_config(CLIENT);
	if (rv < 0) {
		log_error("cannot read config");
		goto out;
	}

	switch (cl.op) {
	case CMD_LIST:
	case CMD_PEERS:
		rv = query_get_string_answer(cl.op);
		break;

	case CMD_GRANT:
	case CMD_REVOKE:
		rv = do_command(cl.op);
		break;
	}

out:
	return rv;
}