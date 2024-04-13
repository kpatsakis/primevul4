lookup_nsap(netdissect_options *ndo, register const u_char *nsap,
	    register u_int nsap_length)
{
	register u_int i, j, k;
	struct enamemem *tp;
	const u_char *ensap;

	if (nsap_length > 6) {
		ensap = nsap + nsap_length - 6;
		k = (ensap[0] << 8) | ensap[1];
		j = (ensap[2] << 8) | ensap[3];
		i = (ensap[4] << 8) | ensap[5];
	}
	else
		i = j = k = 0;

	tp = &nsaptable[(i ^ j) & (HASHNAMESIZE-1)];
	while (tp->e_nxt)
		if (tp->e_addr0 == i &&
		    tp->e_addr1 == j &&
		    tp->e_addr2 == k &&
		    tp->e_nsap[0] == nsap_length &&
		    memcmp((const char *)&(nsap[1]),
			(char *)&(tp->e_nsap[1]), nsap_length) == 0)
			return tp;
		else
			tp = tp->e_nxt;
	tp->e_addr0 = i;
	tp->e_addr1 = j;
	tp->e_addr2 = k;
	tp->e_nsap = (u_char *)malloc(nsap_length + 1);
	if (tp->e_nsap == NULL)
		(*ndo->ndo_error)(ndo, "lookup_nsap: malloc");
	tp->e_nsap[0] = (u_char)nsap_length;	/* guaranteed < ISONSAP_MAX_LENGTH */
	memcpy((char *)&tp->e_nsap[1], (const char *)nsap, nsap_length);
	tp->e_nxt = (struct enamemem *)calloc(1, sizeof(*tp));
	if (tp->e_nxt == NULL)
		(*ndo->ndo_error)(ndo, "lookup_nsap: calloc");

	return tp;
}
