snap_print(netdissect_options *ndo, const u_char *p, u_int length, u_int caplen,
	const struct lladdr_info *src, const struct lladdr_info *dst,
	u_int bridge_pad)
{
	uint32_t orgcode;
	register u_short et;
	register int ret;

	ND_TCHECK2(*p, 5);
	if (caplen < 5 || length < 5)
		goto trunc;
	orgcode = EXTRACT_24BITS(p);
	et = EXTRACT_16BITS(p + 3);

	if (ndo->ndo_eflag) {
		/*
		 * Somebody's already printed the MAC addresses, if there
		 * are any, so just print the SNAP header, not the MAC
		 * addresses.
		 */
		ND_PRINT((ndo, "oui %s (0x%06x), %s %s (0x%04x), length %u: ",
		     tok2str(oui_values, "Unknown", orgcode),
		     orgcode,
		     (orgcode == 0x000000 ? "ethertype" : "pid"),
		     tok2str(oui_to_struct_tok(orgcode), "Unknown", et),
		     et, length - 5));
	}
	p += 5;
	length -= 5;
	caplen -= 5;

	switch (orgcode) {
	case OUI_ENCAP_ETHER:
	case OUI_CISCO_90:
		/*
		 * This is an encapsulated Ethernet packet,
		 * or a packet bridged by some piece of
		 * Cisco hardware; the protocol ID is
		 * an Ethernet protocol type.
		 */
		ret = ethertype_print(ndo, et, p, length, caplen, src, dst);
		if (ret)
			return (ret);
		break;

	case OUI_APPLETALK:
		if (et == ETHERTYPE_ATALK) {
			/*
			 * No, I have no idea why Apple used one
			 * of their own OUIs, rather than
			 * 0x000000, and an Ethernet packet
			 * type, for Appletalk data packets,
			 * but used 0x000000 and an Ethernet
			 * packet type for AARP packets.
			 */
			ret = ethertype_print(ndo, et, p, length, caplen, src, dst);
			if (ret)
				return (ret);
		}
		break;

	case OUI_CISCO:
                switch (et) {
                case PID_CISCO_CDP:
                        cdp_print(ndo, p, length, caplen);
                        return (1);
                case PID_CISCO_DTP:
                        dtp_print(ndo, p, length);
                        return (1);
                case PID_CISCO_UDLD:
                        udld_print(ndo, p, length);
                        return (1);
                case PID_CISCO_VTP:
                        vtp_print(ndo, p, length);
                        return (1);
                case PID_CISCO_PVST:
                case PID_CISCO_VLANBRIDGE:
                        stp_print(ndo, p, length);
                        return (1);
                default:
                        break;
                }
		break;

	case OUI_RFC2684:
		switch (et) {

		case PID_RFC2684_ETH_FCS:
		case PID_RFC2684_ETH_NOFCS:
			/*
			 * XXX - remove the last two bytes for
			 * PID_RFC2684_ETH_FCS?
			 */
			/*
			 * Skip the padding.
			 */
			ND_TCHECK2(*p, bridge_pad);
			caplen -= bridge_pad;
			length -= bridge_pad;
			p += bridge_pad;

			/*
			 * What remains is an Ethernet packet.
			 */
			ether_print(ndo, p, length, caplen, NULL, NULL);
			return (1);

		case PID_RFC2684_802_5_FCS:
		case PID_RFC2684_802_5_NOFCS:
			/*
			 * XXX - remove the last two bytes for
			 * PID_RFC2684_ETH_FCS?
			 */
			/*
			 * Skip the padding, but not the Access
			 * Control field.
			 */
			ND_TCHECK2(*p, bridge_pad);
			caplen -= bridge_pad;
			length -= bridge_pad;
			p += bridge_pad;

			/*
			 * What remains is an 802.5 Token Ring
			 * packet.
			 */
			token_print(ndo, p, length, caplen);
			return (1);

		case PID_RFC2684_FDDI_FCS:
		case PID_RFC2684_FDDI_NOFCS:
			/*
			 * XXX - remove the last two bytes for
			 * PID_RFC2684_ETH_FCS?
			 */
			/*
			 * Skip the padding.
			 */
			ND_TCHECK2(*p, bridge_pad + 1);
			caplen -= bridge_pad + 1;
			length -= bridge_pad + 1;
			p += bridge_pad + 1;

			/*
			 * What remains is an FDDI packet.
			 */
			fddi_print(ndo, p, length, caplen);
			return (1);

		case PID_RFC2684_BPDU:
			stp_print(ndo, p, length);
			return (1);
		}
	}
	if (!ndo->ndo_eflag) {
		/*
		 * Nobody printed the link-layer addresses, so print them, if
		 * we have any.
		 */
		if (src != NULL && dst != NULL) {
			ND_PRINT((ndo, "%s > %s ",
				(src->addr_string)(ndo, src->addr),
				(dst->addr_string)(ndo, dst->addr)));
		}
		/*
		 * Print the SNAP header, but if the OUI is 000000, don't
		 * bother printing it, and report the PID as being an
		 * ethertype.
		 */
		if (orgcode == 0x000000) {
			ND_PRINT((ndo, "SNAP, ethertype %s (0x%04x), length %u: ",
			     tok2str(ethertype_values, "Unknown", et),
			     et, length));
		} else {
			ND_PRINT((ndo, "SNAP, oui %s (0x%06x), pid %s (0x%04x), length %u: ",
			     tok2str(oui_values, "Unknown", orgcode),
			     orgcode,
			     tok2str(oui_to_struct_tok(orgcode), "Unknown", et),
			     et, length));
		}
	}
	return (0);

trunc:
	ND_PRINT((ndo, "[|snap]"));
	return (1);
}
