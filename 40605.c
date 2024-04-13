int pfkey_sadb_addr2xfrm_addr(const struct sadb_address *addr, xfrm_address_t *xaddr)
{
	return pfkey_sockaddr_extract((struct sockaddr *)(addr + 1),
				      xaddr);
}
