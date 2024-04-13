gre_sre_asn_print(netdissect_options *ndo, uint8_t sreoff, uint8_t srelen,
                  const u_char *bp, u_int len)
{
	const u_char *up = bp;

	if (sreoff & 1) {
		ND_PRINT((ndo, ", badoffset=%u", sreoff));
		return (1);
	}
	if (srelen & 1) {
		ND_PRINT((ndo, ", badlength=%u", srelen));
		return (1);
	}
	if (sreoff >= srelen) {
		ND_PRINT((ndo, ", badoff/len=%u/%u", sreoff, srelen));
		return (1);
	}

	while (srelen != 0) {
		if (!ND_TTEST2(*bp, 2))
			return (0);
		if (len < 2)
			return (0);

		ND_PRINT((ndo, " %s%x",
		    ((bp - up) == sreoff) ? "*" : "",
		    EXTRACT_16BITS(bp)));

		bp += 2;
		len -= 2;
		srelen -= 2;
	}
	return (1);
}
