ieee802_11_radio_print(netdissect_options *ndo,
                       const u_char *p, u_int length, u_int caplen)
{
#define	BIT(n)	(1U << n)
#define	IS_EXTENDED(__p)	\
	    (EXTRACT_LE_32BITS(__p) & BIT(IEEE80211_RADIOTAP_EXT)) != 0

	struct cpack_state cpacker;
	const struct ieee80211_radiotap_header *hdr;
	uint32_t presentflags;
	const uint32_t *presentp, *last_presentp;
	int vendor_namespace;
	uint8_t vendor_oui[3];
	uint8_t vendor_subnamespace;
	uint16_t skip_length;
	int bit0;
	u_int len;
	uint8_t flags;
	int pad;
	u_int fcslen;

	if (caplen < sizeof(*hdr)) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	hdr = (const struct ieee80211_radiotap_header *)p;

	len = EXTRACT_LE_16BITS(&hdr->it_len);

	/*
	 * If we don't have the entire radiotap header, just give up.
	 */
	if (caplen < len) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}
	cpack_init(&cpacker, (const uint8_t *)hdr, len); /* align against header start */
	cpack_advance(&cpacker, sizeof(*hdr)); /* includes the 1st bitmap */
	for (last_presentp = &hdr->it_present;
	     (const u_char*)(last_presentp + 1) <= p + len &&
	     IS_EXTENDED(last_presentp);
	     last_presentp++)
	  cpack_advance(&cpacker, sizeof(hdr->it_present)); /* more bitmaps */

	/* are there more bitmap extensions than bytes in header? */
	if ((const u_char*)(last_presentp + 1) > p + len) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	/*
	 * Start out at the beginning of the default radiotap namespace.
	 */
	bit0 = 0;
	vendor_namespace = 0;
	memset(vendor_oui, 0, 3);
	vendor_subnamespace = 0;
	skip_length = 0;
	/* Assume no flags */
	flags = 0;
	/* Assume no Atheros padding between 802.11 header and body */
	pad = 0;
	/* Assume no FCS at end of frame */
	fcslen = 0;
	for (presentp = &hdr->it_present; presentp <= last_presentp;
	    presentp++) {
		presentflags = EXTRACT_LE_32BITS(presentp);

		/*
		 * If this is a vendor namespace, we don't handle it.
		 */
		if (vendor_namespace) {
			/*
			 * Skip past the stuff we don't understand.
			 * If we add support for any vendor namespaces,
			 * it'd be added here; use vendor_oui and
			 * vendor_subnamespace to interpret the fields.
			 */
			if (cpack_advance(&cpacker, skip_length) != 0) {
				/*
				 * Ran out of space in the packet.
				 */
				break;
			}

			/*
			 * We've skipped it all; nothing more to
			 * skip.
			 */
			skip_length = 0;
		} else {
			if (print_in_radiotap_namespace(ndo, &cpacker,
			    &flags, presentflags, bit0) != 0) {
				/*
				 * Fatal error - can't process anything
				 * more in the radiotap header.
				 */
				break;
			}
		}

		/*
		 * Handle the namespace switch bits; we've already handled
		 * the extension bit in all but the last word above.
		 */
		switch (presentflags &
		    (BIT(IEEE80211_RADIOTAP_NAMESPACE)|BIT(IEEE80211_RADIOTAP_VENDOR_NAMESPACE))) {

		case 0:
			/*
			 * We're not changing namespaces.
			 * advance to the next 32 bits in the current
			 * namespace.
			 */
			bit0 += 32;
			break;

		case BIT(IEEE80211_RADIOTAP_NAMESPACE):
			/*
			 * We're switching to the radiotap namespace.
			 * Reset the presence-bitmap index to 0, and
			 * reset the namespace to the default radiotap
			 * namespace.
			 */
			bit0 = 0;
			vendor_namespace = 0;
			memset(vendor_oui, 0, 3);
			vendor_subnamespace = 0;
			skip_length = 0;
			break;

		case BIT(IEEE80211_RADIOTAP_VENDOR_NAMESPACE):
			/*
			 * We're switching to a vendor namespace.
			 * Reset the presence-bitmap index to 0,
			 * note that we're in a vendor namespace,
			 * and fetch the fields of the Vendor Namespace
			 * item.
			 */
			bit0 = 0;
			vendor_namespace = 1;
			if ((cpack_align_and_reserve(&cpacker, 2)) == NULL) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			if (cpack_uint8(&cpacker, &vendor_oui[0]) != 0) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			if (cpack_uint8(&cpacker, &vendor_oui[1]) != 0) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			if (cpack_uint8(&cpacker, &vendor_oui[2]) != 0) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			if (cpack_uint8(&cpacker, &vendor_subnamespace) != 0) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			if (cpack_uint16(&cpacker, &skip_length) != 0) {
				ND_PRINT((ndo, "%s", tstr));
				break;
			}
			break;

		default:
			/*
			 * Illegal combination.  The behavior in this
			 * case is undefined by the radiotap spec; we
			 * just ignore both bits.
			 */
			break;
		}
	}

	if (flags & IEEE80211_RADIOTAP_F_DATAPAD)
		pad = 1;	/* Atheros padding */
	if (flags & IEEE80211_RADIOTAP_F_FCS)
		fcslen = 4;	/* FCS at end of packet */
	return len + ieee802_11_print(ndo, p + len, length - len, caplen - len, pad,
	    fcslen);
#undef BITNO_32
#undef BITNO_16
#undef BITNO_8
#undef BITNO_4
#undef BITNO_2
#undef BIT
}
