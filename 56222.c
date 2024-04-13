parsestatus(netdissect_options *ndo,
            const uint32_t *dp, int *er)
{
	int errnum;

	ND_TCHECK(dp[0]);

	errnum = EXTRACT_32BITS(&dp[0]);
	if (er)
		*er = errnum;
	if (errnum != 0) {
		if (!ndo->ndo_qflag)
			ND_PRINT((ndo, " ERROR: %s",
			    tok2str(status2str, "unk %d", errnum)));
		nfserr = 1;
	}
	return (dp + 1);
trunc:
	return NULL;
}
