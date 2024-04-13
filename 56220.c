parsereq(netdissect_options *ndo,
         register const struct sunrpc_msg *rp, register u_int length)
{
	register const uint32_t *dp;
	register u_int len;

	/*
	 * find the start of the req data (if we captured it)
	 */
	dp = (const uint32_t *)&rp->rm_call.cb_cred;
	ND_TCHECK(dp[1]);
	len = EXTRACT_32BITS(&dp[1]);
	if (len < length) {
		dp += (len + (2 * sizeof(*dp) + 3)) / sizeof(*dp);
		ND_TCHECK(dp[1]);
		len = EXTRACT_32BITS(&dp[1]);
		if (len < length) {
			dp += (len + (2 * sizeof(*dp) + 3)) / sizeof(*dp);
			ND_TCHECK2(dp[0], 0);
			return (dp);
		}
	}
trunc:
	return (NULL);
}
