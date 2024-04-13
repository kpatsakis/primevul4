parseattrstat(netdissect_options *ndo,
              const uint32_t *dp, int verbose, int v3)
{
	int er;

	dp = parsestatus(ndo, dp, &er);
	if (dp == NULL)
		return (0);
	if (er)
		return (1);

	return (parsefattr(ndo, dp, verbose, v3) != NULL);
}
