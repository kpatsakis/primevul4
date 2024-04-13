ns_qprint(netdissect_options *ndo,
          register const u_char *cp, register const u_char *bp, int is_mdns)
{
	register const u_char *np = cp;
	register u_int i, class;

	cp = ns_nskip(ndo, cp);

	if (cp == NULL || !ND_TTEST2(*cp, 4))
		return(NULL);

	/* print the qtype */
	i = EXTRACT_16BITS(cp);
	cp += 2;
	ND_PRINT((ndo, " %s", tok2str(ns_type2str, "Type%d", i)));
	/* print the qclass (if it's not IN) */
	i = EXTRACT_16BITS(cp);
	cp += 2;
	if (is_mdns)
		class = (i & ~C_QU);
	else
		class = i;
	if (class != C_IN)
		ND_PRINT((ndo, " %s", tok2str(ns_class2str, "(Class %d)", class)));
	if (is_mdns) {
		ND_PRINT((ndo, i & C_QU ? " (QU)" : " (QM)"));
	}

	ND_PRINT((ndo, "? "));
	cp = ns_nprint(ndo, np, bp);
	return(cp ? cp + 4 : NULL);
}
