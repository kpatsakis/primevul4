ip_finddst(netdissect_options *ndo,
           const struct ip *ip)
{
	int length;
	int len;
	const u_char *cp;
	uint32_t retval;

	cp = (const u_char *)(ip + 1);
	length = (IP_HL(ip) << 2) - sizeof(struct ip);

	for (; length > 0; cp += len, length -= len) {
		int tt;

		ND_TCHECK(*cp);
		tt = *cp;
		if (tt == IPOPT_EOL)
			break;
		else if (tt == IPOPT_NOP)
			len = 1;
		else {
			ND_TCHECK(cp[1]);
			len = cp[1];
			if (len < 2)
				break;
		}
		ND_TCHECK2(*cp, len);
		switch (tt) {

		case IPOPT_SSRR:
		case IPOPT_LSRR:
			if (len < 7)
				break;
			UNALIGNED_MEMCPY(&retval, cp + len - 4, 4);
			return retval;
		}
	}
trunc:
	UNALIGNED_MEMCPY(&retval, &ip->ip_dst, sizeof(uint32_t));
	return retval;
}
