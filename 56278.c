atm_llc_print(netdissect_options *ndo,
              const u_char *p, int length, int caplen)
{
	int llc_hdrlen;

	llc_hdrlen = llc_print(ndo, p, length, caplen, NULL, NULL);
	if (llc_hdrlen < 0) {
		/* packet not known, print raw packet */
		if (!ndo->ndo_suppress_default_print)
			ND_DEFAULTPRINT(p, caplen);
		llc_hdrlen = -llc_hdrlen;
	}
	return (llc_hdrlen);
}
