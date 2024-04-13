gre_sre_ip_print(netdissect_options *ndo, uint8_t sreoff, uint8_t srelen,
                 const u_char *bp, u_int len)
{
	const u_char *up = bp;
	char buf[INET_ADDRSTRLEN];

	if (sreoff & 3) {
		ND_PRINT((ndo, ", badoffset=%u", sreoff));
		return (1);
	}
	if (srelen & 3) {
		ND_PRINT((ndo, ", badlength=%u", srelen));
		return (1);
	}
	if (sreoff >= srelen) {
		ND_PRINT((ndo, ", badoff/len=%u/%u", sreoff, srelen));
		return (1);
	}

	while (srelen != 0) {
		if (!ND_TTEST2(*bp, 4))
			return (0);
		if (len < 4)
			return (0);

		addrtostr(bp, buf, sizeof(buf));
		ND_PRINT((ndo, " %s%s",
		    ((bp - up) == sreoff) ? "*" : "", buf));

		bp += 4;
		len -= 4;
		srelen -= 4;
	}
	return (1);
}
