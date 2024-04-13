xid_map_find(const struct sunrpc_msg *rp, const u_char *bp, uint32_t *proc,
	     uint32_t *vers)
{
	int i;
	struct xid_map_entry *xmep;
	uint32_t xid;
	const struct ip *ip = (const struct ip *)bp;
	const struct ip6_hdr *ip6 = (const struct ip6_hdr *)bp;
	int cmp;

	UNALIGNED_MEMCPY(&xid, &rp->rm_xid, sizeof(xmep->xid));
	/* Start searching from where we last left off */
	i = xid_map_hint;
	do {
		xmep = &xid_map[i];
		cmp = 1;
		if (xmep->ipver != IP_V(ip) || xmep->xid != xid)
			goto nextitem;
		switch (xmep->ipver) {
		case 4:
			if (UNALIGNED_MEMCMP(&ip->ip_src, &xmep->server,
				   sizeof(ip->ip_src)) != 0 ||
			    UNALIGNED_MEMCMP(&ip->ip_dst, &xmep->client,
				   sizeof(ip->ip_dst)) != 0) {
				cmp = 0;
			}
			break;
		case 6:
			if (UNALIGNED_MEMCMP(&ip6->ip6_src, &xmep->server,
				   sizeof(ip6->ip6_src)) != 0 ||
			    UNALIGNED_MEMCMP(&ip6->ip6_dst, &xmep->client,
				   sizeof(ip6->ip6_dst)) != 0) {
				cmp = 0;
			}
			break;
		default:
			cmp = 0;
			break;
		}
		if (cmp) {
			/* match */
			xid_map_hint = i;
			*proc = xmep->proc;
			*vers = xmep->vers;
			return 0;
		}
	nextitem:
		if (++i >= XIDMAPSIZE)
			i = 0;
	} while (i != xid_map_hint);

	/* search failed */
	return (-1);
}
