static sctp_scope_t sctp_v4_scope(union sctp_addr *addr)
{
	sctp_scope_t retval;

	/* Check for unusable SCTP addresses. */
	if (IS_IPV4_UNUSABLE_ADDRESS(addr->v4.sin_addr.s_addr)) {
		retval =  SCTP_SCOPE_UNUSABLE;
	} else if (ipv4_is_loopback(addr->v4.sin_addr.s_addr)) {
		retval = SCTP_SCOPE_LOOPBACK;
	} else if (ipv4_is_linklocal_169(addr->v4.sin_addr.s_addr)) {
		retval = SCTP_SCOPE_LINK;
	} else if (ipv4_is_private_10(addr->v4.sin_addr.s_addr) ||
		   ipv4_is_private_172(addr->v4.sin_addr.s_addr) ||
		   ipv4_is_private_192(addr->v4.sin_addr.s_addr)) {
		retval = SCTP_SCOPE_PRIVATE;
	} else {
		retval = SCTP_SCOPE_GLOBAL;
	}

	return retval;
}
