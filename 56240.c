ns_cprint(netdissect_options *ndo,
          register const u_char *cp)
{
	register u_int i;

	if (!ND_TTEST2(*cp, 1))
		return (NULL);
	i = *cp++;
	if (fn_printn(ndo, cp, i, ndo->ndo_snapend))
		return (NULL);
	return (cp + i);
}
