ospf6_decode_v3_trailer(netdissect_options *ndo,
                        const struct ospf6hdr *op, const u_char *cp, const unsigned len)
{
	int llslen = 0;
	int lls_hello = 0;
	int lls_dd = 0;

	if (op->ospf6_type == OSPF_TYPE_HELLO) {
		const struct hello6 *hellop = (const struct hello6 *)((const uint8_t *)op + OSPF6HDR_LEN);
		if (EXTRACT_32BITS(&hellop->hello_options) & OSPF6_OPTION_L)
			lls_hello = 1;
	} else if (op->ospf6_type == OSPF_TYPE_DD) {
		const struct dd6 *ddp = (const struct dd6 *)((const uint8_t *)op + OSPF6HDR_LEN);
		if (EXTRACT_32BITS(&ddp->db_options) & OSPF6_OPTION_L)
			lls_dd = 1;
	}
	if ((lls_hello || lls_dd) && (llslen = ospf6_print_lls(ndo, cp, len)) < 0)
		goto trunc;
	return ospf6_decode_at(ndo, cp + llslen, len - llslen);

trunc:
	return 1;
}
