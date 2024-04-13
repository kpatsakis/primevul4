ns_nskip(netdissect_options *ndo,
         register const u_char *cp)
{
	register u_char i;

	if (!ND_TTEST2(*cp, 1))
		return (NULL);
	i = *cp++;
	while (i) {
		if ((i & INDIR_MASK) == INDIR_MASK)
			return (cp + 1);
		if ((i & INDIR_MASK) == EDNS0_MASK) {
			int bitlen, bytelen;

			if ((i & ~INDIR_MASK) != EDNS0_ELT_BITLABEL)
				return(NULL); /* unknown ELT */
			if (!ND_TTEST2(*cp, 1))
				return (NULL);
			if ((bitlen = *cp++) == 0)
				bitlen = 256;
			bytelen = (bitlen + 7) / 8;
			cp += bytelen;
		} else
			cp += i;
		if (!ND_TTEST2(*cp, 1))
			return (NULL);
		i = *cp++;
	}
	return (cp);
}
