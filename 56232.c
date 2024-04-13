aodv_v6_draft_01_rerr(netdissect_options *ndo, const u_char *dat, u_int length)
{
	u_int i, dc;
	const struct aodv_rerr *ap = (const struct aodv_rerr *)dat;
	const struct rerr_unreach6_draft_01 *dp6;

	ND_TCHECK(*ap);
	if (length < sizeof(*ap))
		goto trunc;
	ND_PRINT((ndo, " rerr %s [items %u] [%u]:",
	    ap->rerr_flags & RERR_NODELETE ? "[D]" : "",
	    ap->rerr_dc, length));
	dp6 = (const struct rerr_unreach6_draft_01 *)(const void *)(ap + 1);
	i = length - sizeof(*ap);
	for (dc = ap->rerr_dc; dc != 0; dc--) {
		ND_TCHECK(*dp6);
		if (i < sizeof(*dp6))
			goto trunc;
		ND_PRINT((ndo, " {%s}(%ld)", ip6addr_string(ndo, &dp6->u_da),
		    (unsigned long)EXTRACT_32BITS(&dp6->u_ds)));
		dp6++;
		i -= sizeof(*dp6);
	}
	return;

trunc:
	ND_PRINT((ndo, "[|rerr]"));
}
