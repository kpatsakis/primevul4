rip6_entry_print(netdissect_options *ndo, register const struct netinfo6 *ni, int metric)
{
	int l;
	l = ND_PRINT((ndo, "%s/%d", ip6addr_string(ndo, &ni->rip6_dest), ni->rip6_plen));
	if (ni->rip6_tag)
		l += ND_PRINT((ndo, " [%d]", EXTRACT_16BITS(&ni->rip6_tag)));
	if (metric)
		l += ND_PRINT((ndo, " (%d)", ni->rip6_metric));
	return l;
}
