static bool list_has_sctp_addr(const struct list_head *list,
			       union sctp_addr *ipaddr)
{
	struct sctp_transport *addr;

	list_for_each_entry(addr, list, transports) {
		if (sctp_cmp_addr_exact(ipaddr, &addr->ipaddr))
			return true;
	}

	return false;
}
