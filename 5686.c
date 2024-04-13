static int setup_transport(void)
{
	int rv;

	rv = transport()->init(message_recv);
	if (rv < 0) {
		log_error("failed to init booth_transport %s", transport()->name);
		goto out;
	}

	rv = booth_transport[TCP].init(NULL);
	if (rv < 0) {
		log_error("failed to init booth_transport[TCP]");
		goto out;
	}

out:
	return rv;
}