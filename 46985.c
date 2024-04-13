int sctp_register_af(struct sctp_af *af)
{
	switch (af->sa_family) {
	case AF_INET:
		if (sctp_af_v4_specific)
			return 0;
		sctp_af_v4_specific = af;
		break;
	case AF_INET6:
		if (sctp_af_v6_specific)
			return 0;
		sctp_af_v6_specific = af;
		break;
	default:
		return 0;
	}

	INIT_LIST_HEAD(&af->list);
	list_add_tail(&af->list, &sctp_address_families);
	return 1;
}
