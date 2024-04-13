parsestatfs(netdissect_options *ndo,
            const uint32_t *dp, int v3)
{
	const struct nfs_statfs *sfsp;
	int er;

	dp = parsestatus(ndo, dp, &er);
	if (dp == NULL)
		return (0);
	if (!v3 && er)
		return (1);

	if (ndo->ndo_qflag)
		return(1);

	if (v3) {
		if (ndo->ndo_vflag)
			ND_PRINT((ndo, " POST:"));
		if (!(dp = parse_post_op_attr(ndo, dp, ndo->ndo_vflag)))
			return (0);
	}

	ND_TCHECK2(*dp, (v3 ? NFSX_V3STATFS : NFSX_V2STATFS));

	sfsp = (const struct nfs_statfs *)dp;

	if (v3) {
		ND_PRINT((ndo, " tbytes %" PRIu64 " fbytes %" PRIu64 " abytes %" PRIu64,
			EXTRACT_64BITS((const uint32_t *)&sfsp->sf_tbytes),
			EXTRACT_64BITS((const uint32_t *)&sfsp->sf_fbytes),
			EXTRACT_64BITS((const uint32_t *)&sfsp->sf_abytes)));
		if (ndo->ndo_vflag) {
			ND_PRINT((ndo, " tfiles %" PRIu64 " ffiles %" PRIu64 " afiles %" PRIu64 " invar %u",
			       EXTRACT_64BITS((const uint32_t *)&sfsp->sf_tfiles),
			       EXTRACT_64BITS((const uint32_t *)&sfsp->sf_ffiles),
			       EXTRACT_64BITS((const uint32_t *)&sfsp->sf_afiles),
			       EXTRACT_32BITS(&sfsp->sf_invarsec)));
		}
	} else {
		ND_PRINT((ndo, " tsize %d bsize %d blocks %d bfree %d bavail %d",
			EXTRACT_32BITS(&sfsp->sf_tsize),
			EXTRACT_32BITS(&sfsp->sf_bsize),
			EXTRACT_32BITS(&sfsp->sf_blocks),
			EXTRACT_32BITS(&sfsp->sf_bfree),
			EXTRACT_32BITS(&sfsp->sf_bavail)));
	}

	return (1);
trunc:
	return (0);
}
