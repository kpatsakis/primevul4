handle_beacon(netdissect_options *ndo,
              const u_char *p, u_int length)
{
	struct mgmt_body_t pbody;
	int offset = 0;
	int ret;

	memset(&pbody, 0, sizeof(pbody));

	if (!ND_TTEST2(*p, IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN +
	    IEEE802_11_CAPINFO_LEN))
		return 0;
	if (length < IEEE802_11_TSTAMP_LEN + IEEE802_11_BCNINT_LEN +
	    IEEE802_11_CAPINFO_LEN)
		return 0;
	memcpy(&pbody.timestamp, p, IEEE802_11_TSTAMP_LEN);
	offset += IEEE802_11_TSTAMP_LEN;
	length -= IEEE802_11_TSTAMP_LEN;
	pbody.beacon_interval = EXTRACT_LE_16BITS(p+offset);
	offset += IEEE802_11_BCNINT_LEN;
	length -= IEEE802_11_BCNINT_LEN;
	pbody.capability_info = EXTRACT_LE_16BITS(p+offset);
	offset += IEEE802_11_CAPINFO_LEN;
	length -= IEEE802_11_CAPINFO_LEN;

	ret = parse_elements(ndo, &pbody, p, offset, length);

	PRINT_SSID(pbody);
	PRINT_RATES(pbody);
	ND_PRINT((ndo, " %s",
	    CAPABILITY_ESS(pbody.capability_info) ? "ESS" : "IBSS"));
	PRINT_DS_CHANNEL(pbody);

	return ret;
}
