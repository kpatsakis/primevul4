aodv_v6_rrep(netdissect_options *ndo, const u_char *dat, u_int length)
{
	u_int i;
	const struct aodv_rrep6 *ap = (const struct aodv_rrep6 *)dat;

	ND_TCHECK(*ap);
	if (length < sizeof(*ap))
		goto trunc;
	ND_PRINT((ndo, " rrep %u %s%sprefix %u hops %u\n"
	   "\tdst %s dseq %lu src %s %lu ms", length,
	    ap->rrep_type & RREP_REPAIR ? "[R]" : "",
	    ap->rrep_type & RREP_ACK ? "[A] " : " ",
	    ap->rrep_ps & RREP_PREFIX_MASK,
	    ap->rrep_hops,
	    ip6addr_string(ndo, &ap->rrep_da),
	    (unsigned long)EXTRACT_32BITS(&ap->rrep_ds),
	    ip6addr_string(ndo, &ap->rrep_oa),
	    (unsigned long)EXTRACT_32BITS(&ap->rrep_life)));
	i = length - sizeof(*ap);
	if (i >= sizeof(struct aodv_ext))
		aodv_extension(ndo, (const struct aodv_ext *)(dat + sizeof(*ap)), i);
	return;

trunc:
	ND_PRINT((ndo, " [|rreq"));
}
