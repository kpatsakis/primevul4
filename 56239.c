labellen(netdissect_options *ndo,
         const u_char *cp)
{
	register u_int i;

	if (!ND_TTEST2(*cp, 1))
		return(-1);
	i = *cp;
	if ((i & INDIR_MASK) == EDNS0_MASK) {
		int bitlen, elt;
		if ((elt = (i & ~INDIR_MASK)) != EDNS0_ELT_BITLABEL) {
			ND_PRINT((ndo, "<ELT %d>", elt));
			return(-1);
		}
		if (!ND_TTEST2(*(cp + 1), 1))
			return(-1);
		if ((bitlen = *(cp + 1)) == 0)
			bitlen = 256;
		return(((bitlen + 7) / 8) + 1);
	} else
		return(i);
}
