aodv_rerr(netdissect_options *ndo, const u_char *dat, u_int length)
{
	u_int i, dc;
	const struct aodv_rerr *ap = (const struct aodv_rerr *)dat;
	const struct rerr_unreach *dp;

	ND_TCHECK(*ap);
	if (length < sizeof(*ap))
		goto trunc;
	ND_PRINT((ndo, " rerr %s [items %u] [%u]:",
	    ap->rerr_flags & RERR_NODELETE ? "[D]" : "",
	    ap->rerr_dc, length));
	dp = (const struct rerr_unreach *)(dat + sizeof(*ap));
	i = length - sizeof(*ap);
	for (dc = ap->rerr_dc; dc != 0; dc--) {
		ND_TCHECK(*dp);
		if (i < sizeof(*dp))
			goto trunc;
		ND_PRINT((ndo, " {%s}(%ld)", ipaddr_string(ndo, &dp->u_da),
		    (unsigned long)EXTRACT_32BITS(&dp->u_ds)));
		dp++;
		i -= sizeof(*dp);
	}
	return;

trunc:
	ND_PRINT((ndo, "[|rerr]"));
}
