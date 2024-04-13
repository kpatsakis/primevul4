parsediropres(netdissect_options *ndo,
              const uint32_t *dp)
{
	int er;

	if (!(dp = parsestatus(ndo, dp, &er)))
		return (0);
	if (er)
		return (1);

	dp = parsefh(ndo, dp, 0);
	if (dp == NULL)
		return (0);

	return (parsefattr(ndo, dp, ndo->ndo_vflag, 0) != NULL);
}
