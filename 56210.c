parse_wcc_data(netdissect_options *ndo,
               const uint32_t *dp, int verbose)
{
	if (verbose > 1)
		ND_PRINT((ndo, " PRE:"));
	if (!(dp = parse_pre_op_attr(ndo, dp, verbose)))
		return (0);

	if (verbose)
		ND_PRINT((ndo, " POST:"));
	return parse_post_op_attr(ndo, dp, verbose);
}
