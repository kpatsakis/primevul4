parsepathconf(netdissect_options *ndo,
              const uint32_t *dp)
{
	int er;
	const struct nfsv3_pathconf *spp;

	if (!(dp = parsestatus(ndo, dp, &er)))
		return (0);
	if (ndo->ndo_vflag)
		ND_PRINT((ndo, " POST:"));
	if (!(dp = parse_post_op_attr(ndo, dp, ndo->ndo_vflag)))
		return (0);
	if (er)
		return (1);

	spp = (const struct nfsv3_pathconf *)dp;
	ND_TCHECK(*spp);

	ND_PRINT((ndo, " linkmax %u namemax %u %s %s %s %s",
	       EXTRACT_32BITS(&spp->pc_linkmax),
	       EXTRACT_32BITS(&spp->pc_namemax),
	       EXTRACT_32BITS(&spp->pc_notrunc) ? "notrunc" : "",
	       EXTRACT_32BITS(&spp->pc_chownrestricted) ? "chownres" : "",
	       EXTRACT_32BITS(&spp->pc_caseinsensitive) ? "igncase" : "",
	       EXTRACT_32BITS(&spp->pc_casepreserving) ? "keepcase" : ""));
	return (1);
trunc:
	return (0);
}
