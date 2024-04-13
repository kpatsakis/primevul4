parse_sattr3(netdissect_options *ndo,
             const uint32_t *dp, struct nfsv3_sattr *sa3)
{
	ND_TCHECK(dp[0]);
	sa3->sa_modeset = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_modeset) {
		ND_TCHECK(dp[0]);
		sa3->sa_mode = EXTRACT_32BITS(dp);
		dp++;
	}

	ND_TCHECK(dp[0]);
	sa3->sa_uidset = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_uidset) {
		ND_TCHECK(dp[0]);
		sa3->sa_uid = EXTRACT_32BITS(dp);
		dp++;
	}

	ND_TCHECK(dp[0]);
	sa3->sa_gidset = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_gidset) {
		ND_TCHECK(dp[0]);
		sa3->sa_gid = EXTRACT_32BITS(dp);
		dp++;
	}

	ND_TCHECK(dp[0]);
	sa3->sa_sizeset = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_sizeset) {
		ND_TCHECK(dp[0]);
		sa3->sa_size = EXTRACT_32BITS(dp);
		dp++;
	}

	ND_TCHECK(dp[0]);
	sa3->sa_atimetype = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_atimetype == NFSV3SATTRTIME_TOCLIENT) {
		ND_TCHECK(dp[1]);
		sa3->sa_atime.nfsv3_sec = EXTRACT_32BITS(dp);
		dp++;
		sa3->sa_atime.nfsv3_nsec = EXTRACT_32BITS(dp);
		dp++;
	}

	ND_TCHECK(dp[0]);
	sa3->sa_mtimetype = EXTRACT_32BITS(dp);
	dp++;
	if (sa3->sa_mtimetype == NFSV3SATTRTIME_TOCLIENT) {
		ND_TCHECK(dp[1]);
		sa3->sa_mtime.nfsv3_sec = EXTRACT_32BITS(dp);
		dp++;
		sa3->sa_mtime.nfsv3_nsec = EXTRACT_32BITS(dp);
		dp++;
	}

	return dp;
trunc:
	return NULL;
}
