parsefhn(netdissect_options *ndo,
         register const uint32_t *dp, int v3)
{
	dp = parsefh(ndo, dp, v3);
	if (dp == NULL)
		return (NULL);
	ND_PRINT((ndo, " "));
	return (parsefn(ndo, dp));
}
