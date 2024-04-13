prism_if_print(netdissect_options *ndo,
               const struct pcap_pkthdr *h, const u_char *p)
{
	u_int caplen = h->caplen;
	u_int length = h->len;
	uint32_t msgcode;

	if (caplen < 4) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	msgcode = EXTRACT_32BITS(p);
	if (msgcode == WLANCAP_MAGIC_COOKIE_V1 ||
	    msgcode == WLANCAP_MAGIC_COOKIE_V2)
		return ieee802_11_avs_radio_print(ndo, p, length, caplen);

	if (caplen < PRISM_HDR_LEN) {
		ND_PRINT((ndo, "%s", tstr));
		return caplen;
	}

	return PRISM_HDR_LEN + ieee802_11_print(ndo, p + PRISM_HDR_LEN,
	    length - PRISM_HDR_LEN, caplen - PRISM_HDR_LEN, 0, 0);
}
