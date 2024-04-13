aodv_rreq(netdissect_options *ndo, const u_char *dat, u_int length)
{
	u_int i;
	const struct aodv_rreq *ap = (const struct aodv_rreq *)dat;

	ND_TCHECK(*ap);
	if (length < sizeof(*ap))
		goto trunc;
	ND_PRINT((ndo, " rreq %u %s%s%s%s%shops %u id 0x%08lx\n"
	    "\tdst %s seq %lu src %s seq %lu", length,
	    ap->rreq_type & RREQ_JOIN ? "[J]" : "",
	    ap->rreq_type & RREQ_REPAIR ? "[R]" : "",
	    ap->rreq_type & RREQ_GRAT ? "[G]" : "",
	    ap->rreq_type & RREQ_DEST ? "[D]" : "",
	    ap->rreq_type & RREQ_UNKNOWN ? "[U] " : " ",
	    ap->rreq_hops,
	    (unsigned long)EXTRACT_32BITS(&ap->rreq_id),
	    ipaddr_string(ndo, &ap->rreq_da),
	    (unsigned long)EXTRACT_32BITS(&ap->rreq_ds),
	    ipaddr_string(ndo, &ap->rreq_oa),
	    (unsigned long)EXTRACT_32BITS(&ap->rreq_os)));
	i = length - sizeof(*ap);
	if (i >= sizeof(struct aodv_ext))
		aodv_extension(ndo, (const struct aodv_ext *)(dat + sizeof(*ap)), i);
	return;

trunc:
	ND_PRINT((ndo, " [|rreq"));
}
