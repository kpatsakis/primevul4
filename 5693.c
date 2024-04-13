static int do_attr(void)
{
	int rv = -1;

	rv = setup_config(GEOSTORE);
	if (rv < 0) {
		log_error("cannot read config");
		goto out;
	}

	/* We don't check for existence of ticket, so that asking can be
	 * done without local configuration, too.
	 * Although, that means that the UDP port has to be specified, too. */
	if (!cl.attr_msg.attr.tkt_id[0]) {
		/* If the loaded configuration has only a single ticket defined, use that. */
		if (booth_conf->ticket_count == 1) {
			strncpy(cl.attr_msg.attr.tkt_id, booth_conf->ticket[0].name,
				sizeof(cl.attr_msg.attr.tkt_id));
		} else {
			rv = 1;
			log_error("No ticket given.");
			goto out;
		}
	}

	switch (cl.op) {
	case ATTR_LIST:
	case ATTR_GET:
		rv = query_get_string_answer(cl.op);
		break;

	case ATTR_SET:
	case ATTR_DEL:
		rv = do_attr_command(cl.op);
		break;
	}

out:
	return rv;
}