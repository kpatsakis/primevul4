ether_print(netdissect_options *ndo,
            const u_char *p, u_int length, u_int caplen,
            void (*print_encap_header)(netdissect_options *ndo, const u_char *), const u_char *encap_header_arg)
{
	const struct ether_header *ep;
	u_int orig_length;
	u_short length_type;
	u_int hdrlen;
	int llc_hdrlen;
	struct lladdr_info src, dst;

	if (caplen < ETHER_HDRLEN) {
		ND_PRINT((ndo, "[|ether]"));
		return (caplen);
	}
	if (length < ETHER_HDRLEN) {
		ND_PRINT((ndo, "[|ether]"));
		return (length);
	}

	if (ndo->ndo_eflag) {
		if (print_encap_header != NULL)
			(*print_encap_header)(ndo, encap_header_arg);
		ether_hdr_print(ndo, p, length);
	}
	orig_length = length;

	length -= ETHER_HDRLEN;
	caplen -= ETHER_HDRLEN;
	ep = (const struct ether_header *)p;
	p += ETHER_HDRLEN;
	hdrlen = ETHER_HDRLEN;

	src.addr = ESRC(ep);
	src.addr_string = etheraddr_string;
	dst.addr = EDST(ep);
	dst.addr_string = etheraddr_string;
	length_type = EXTRACT_16BITS(&ep->ether_length_type);

recurse:
	/*
	 * Is it (gag) an 802.3 encapsulation?
	 */
	if (length_type <= ETHERMTU) {
		/* Try to print the LLC-layer header & higher layers */
		llc_hdrlen = llc_print(ndo, p, length, caplen, &src, &dst);
		if (llc_hdrlen < 0) {
			/* packet type not known, print raw packet */
			if (!ndo->ndo_suppress_default_print)
				ND_DEFAULTPRINT(p, caplen);
			llc_hdrlen = -llc_hdrlen;
		}
		hdrlen += llc_hdrlen;
	} else if (length_type == ETHERTYPE_8021Q  ||
                length_type == ETHERTYPE_8021Q9100 ||
                length_type == ETHERTYPE_8021Q9200 ||
                length_type == ETHERTYPE_8021QinQ) {
		/*
		 * Print VLAN information, and then go back and process
		 * the enclosed type field.
		 */
		if (caplen < 4) {
			ND_PRINT((ndo, "[|vlan]"));
			return (hdrlen + caplen);
		}
		if (length < 4) {
			ND_PRINT((ndo, "[|vlan]"));
			return (hdrlen + length);
		}
	        if (ndo->ndo_eflag) {
			uint16_t tag = EXTRACT_16BITS(p);

			ND_PRINT((ndo, "%s, ", ieee8021q_tci_string(tag)));
		}

		length_type = EXTRACT_16BITS(p + 2);
		if (ndo->ndo_eflag && length_type > ETHERMTU)
			ND_PRINT((ndo, "ethertype %s, ", tok2str(ethertype_values,"0x%04x", length_type)));
		p += 4;
		length -= 4;
		caplen -= 4;
		hdrlen += 4;
		goto recurse;
	} else if (length_type == ETHERTYPE_JUMBO) {
		/*
		 * Alteon jumbo frames.
		 * See
		 *
		 *	http://tools.ietf.org/html/draft-ietf-isis-ext-eth-01
		 *
		 * which indicates that, following the type field,
		 * there's an LLC header and payload.
		 */
		/* Try to print the LLC-layer header & higher layers */
		llc_hdrlen = llc_print(ndo, p, length, caplen, &src, &dst);
		if (llc_hdrlen < 0) {
			/* packet type not known, print raw packet */
			if (!ndo->ndo_suppress_default_print)
				ND_DEFAULTPRINT(p, caplen);
			llc_hdrlen = -llc_hdrlen;
		}
		hdrlen += llc_hdrlen;
	} else {
		if (ethertype_print(ndo, length_type, p, length, caplen, &src, &dst) == 0) {
			/* type not known, print raw packet */
			if (!ndo->ndo_eflag) {
				if (print_encap_header != NULL)
					(*print_encap_header)(ndo, encap_header_arg);
				ether_hdr_print(ndo, (const u_char *)ep, orig_length);
			}

			if (!ndo->ndo_suppress_default_print)
				ND_DEFAULTPRINT(p, caplen);
		}
	}
	return (hdrlen);
}
