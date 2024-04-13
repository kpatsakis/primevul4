static void cma_iboe_set_mgid(struct sockaddr *addr, union ib_gid *mgid)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)addr;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)addr;

	if (cma_any_addr(addr)) {
		memset(mgid, 0, sizeof *mgid);
	} else if (addr->sa_family == AF_INET6) {
		memcpy(mgid, &sin6->sin6_addr, sizeof *mgid);
	} else {
		mgid->raw[0] = 0xff;
		mgid->raw[1] = 0x0e;
		mgid->raw[2] = 0;
		mgid->raw[3] = 0;
		mgid->raw[4] = 0;
		mgid->raw[5] = 0;
		mgid->raw[6] = 0;
		mgid->raw[7] = 0;
		mgid->raw[8] = 0;
		mgid->raw[9] = 0;
		mgid->raw[10] = 0xff;
		mgid->raw[11] = 0xff;
		*(__be32 *)(&mgid->raw[12]) = sin->sin_addr.s_addr;
	}
}
