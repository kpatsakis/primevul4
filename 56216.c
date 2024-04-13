parsefsinfo(netdissect_options *ndo,
            const uint32_t *dp)
{
	const struct nfsv3_fsinfo *sfp;
	int er;

	if (!(dp = parsestatus(ndo, dp, &er)))
		return (0);
	if (ndo->ndo_vflag)
		ND_PRINT((ndo, " POST:"));
	if (!(dp = parse_post_op_attr(ndo, dp, ndo->ndo_vflag)))
		return (0);
	if (er)
		return (1);

	sfp = (const struct nfsv3_fsinfo *)dp;
	ND_TCHECK(*sfp);
	ND_PRINT((ndo, " rtmax %u rtpref %u wtmax %u wtpref %u dtpref %u",
	       EXTRACT_32BITS(&sfp->fs_rtmax),
	       EXTRACT_32BITS(&sfp->fs_rtpref),
	       EXTRACT_32BITS(&sfp->fs_wtmax),
	       EXTRACT_32BITS(&sfp->fs_wtpref),
	       EXTRACT_32BITS(&sfp->fs_dtpref)));
	if (ndo->ndo_vflag) {
		ND_PRINT((ndo, " rtmult %u wtmult %u maxfsz %" PRIu64,
		       EXTRACT_32BITS(&sfp->fs_rtmult),
		       EXTRACT_32BITS(&sfp->fs_wtmult),
		       EXTRACT_64BITS((const uint32_t *)&sfp->fs_maxfilesize)));
		ND_PRINT((ndo, " delta %u.%06u ",
		       EXTRACT_32BITS(&sfp->fs_timedelta.nfsv3_sec),
		       EXTRACT_32BITS(&sfp->fs_timedelta.nfsv3_nsec)));
	}
	return (1);
trunc:
	return (0);
}
