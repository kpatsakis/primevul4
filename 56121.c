mld6_print(netdissect_options *ndo, const u_char *bp)
{
	const struct mld6_hdr *mp = (const struct mld6_hdr *)bp;
	const u_char *ep;

	/* 'ep' points to the end of available data. */
	ep = ndo->ndo_snapend;

	if ((const u_char *)mp + sizeof(*mp) > ep)
		return;

	ND_PRINT((ndo,"max resp delay: %d ", EXTRACT_16BITS(&mp->mld6_maxdelay)));
	ND_PRINT((ndo,"addr: %s", ip6addr_string(ndo, &mp->mld6_addr)));
}
