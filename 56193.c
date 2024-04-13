ieee802_11_avs_radio_print(netdissect_options *ndo,
                           const u_char *p, u_int length, u_int caplen)
{
	uint32_t caphdr_len;

	if (caplen < 8) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	caphdr_len = EXTRACT_32BITS(p + 4);
	if (caphdr_len < 8) {
		/*
		 * Yow!  The capture header length is claimed not
		 * to be large enough to include even the version
		 * cookie or capture header length!
		 */
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	if (caplen < caphdr_len) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	return caphdr_len + ieee802_11_print(ndo, p + caphdr_len,
	    length - caphdr_len, caplen - caphdr_len, 0, 0);
}
