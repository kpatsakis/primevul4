parserddires(netdissect_options *ndo,
             const uint32_t *dp)
{
	int er;

	dp = parsestatus(ndo, dp, &er);
	if (dp == NULL)
		return (0);
	if (er)
		return (1);
	if (ndo->ndo_qflag)
		return (1);

	ND_TCHECK(dp[2]);
	ND_PRINT((ndo, " offset 0x%x size %d ",
	       EXTRACT_32BITS(&dp[0]), EXTRACT_32BITS(&dp[1])));
	if (dp[2] != 0)
		ND_PRINT((ndo, " eof"));

	return (1);
trunc:
	return (0);
}
