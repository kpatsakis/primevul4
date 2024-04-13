parsewccres(netdissect_options *ndo,
            const uint32_t *dp, int verbose)
{
	int er;

	if (!(dp = parsestatus(ndo, dp, &er)))
		return (0);
	return parse_wcc_data(ndo, dp, verbose) != NULL;
}
