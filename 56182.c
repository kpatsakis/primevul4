handle_assoc_request(netdissect_options *ndo,
                     const u_char *p, u_int length)
{
	struct mgmt_body_t pbody;
	int offset = 0;
	int ret;

	memset(&pbody, 0, sizeof(pbody));

	if (!ND_TTEST2(*p, IEEE802_11_CAPINFO_LEN + IEEE802_11_LISTENINT_LEN))
		return 0;
	if (length < IEEE802_11_CAPINFO_LEN + IEEE802_11_LISTENINT_LEN)
		return 0;
	pbody.capability_info = EXTRACT_LE_16BITS(p);
	offset += IEEE802_11_CAPINFO_LEN;
	length -= IEEE802_11_CAPINFO_LEN;
	pbody.listen_interval = EXTRACT_LE_16BITS(p+offset);
	offset += IEEE802_11_LISTENINT_LEN;
	length -= IEEE802_11_LISTENINT_LEN;

	ret = parse_elements(ndo, &pbody, p, offset, length);

	PRINT_SSID(pbody);
	PRINT_RATES(pbody);
	return ret;
}
