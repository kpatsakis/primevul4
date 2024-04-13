ether_hdr_print(netdissect_options *ndo,
                const u_char *bp, u_int length)
{
	register const struct ether_header *ep;
	uint16_t length_type;

	ep = (const struct ether_header *)bp;

	ND_PRINT((ndo, "%s > %s",
		     etheraddr_string(ndo, ESRC(ep)),
		     etheraddr_string(ndo, EDST(ep))));

	length_type = EXTRACT_16BITS(&ep->ether_length_type);
	if (!ndo->ndo_qflag) {
	        if (length_type <= ETHERMTU) {
		        ND_PRINT((ndo, ", 802.3"));
			length = length_type;
		} else
		        ND_PRINT((ndo, ", ethertype %s (0x%04x)",
				       tok2str(ethertype_values,"Unknown", length_type),
                                       length_type));
        } else {
                if (length_type <= ETHERMTU) {
                        ND_PRINT((ndo, ", 802.3"));
			length = length_type;
		} else
                        ND_PRINT((ndo, ", %s", tok2str(ethertype_values,"Unknown Ethertype (0x%04x)", length_type)));
        }

	ND_PRINT((ndo, ", length %u: ", length));
}
