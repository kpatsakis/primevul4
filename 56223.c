parsev3rddirres(netdissect_options *ndo,
                const uint32_t *dp, int verbose)
{
	int er;

	if (!(dp = parsestatus(ndo, dp, &er)))
		return (0);
	if (ndo->ndo_vflag)
		ND_PRINT((ndo, " POST:"));
	if (!(dp = parse_post_op_attr(ndo, dp, verbose)))
		return (0);
	if (er)
		return dp;
	if (ndo->ndo_vflag) {
		ND_TCHECK(dp[1]);
		ND_PRINT((ndo, " verf %08x%08x", dp[0], dp[1]));
		dp += 2;
	}
	return dp;
trunc:
	return (NULL);
}
