handle_assoc_response(netdissect_options *ndo,
                      const u_char *p, u_int length)
{
	struct mgmt_body_t pbody;
	int offset = 0;
	int ret;

	memset(&pbody, 0, sizeof(pbody));

	if (!ND_TTEST2(*p, IEEE802_11_CAPINFO_LEN + IEEE802_11_STATUS_LEN +
	    IEEE802_11_AID_LEN))
		return 0;
	if (length < IEEE802_11_CAPINFO_LEN + IEEE802_11_STATUS_LEN +
	    IEEE802_11_AID_LEN)
		return 0;
	pbody.capability_info = EXTRACT_LE_16BITS(p);
	offset += IEEE802_11_CAPINFO_LEN;
	length -= IEEE802_11_CAPINFO_LEN;
	pbody.status_code = EXTRACT_LE_16BITS(p+offset);
	offset += IEEE802_11_STATUS_LEN;
	length -= IEEE802_11_STATUS_LEN;
	pbody.aid = EXTRACT_LE_16BITS(p+offset);
	offset += IEEE802_11_AID_LEN;
	length -= IEEE802_11_AID_LEN;

	ret = parse_elements(ndo, &pbody, p, offset, length);

	ND_PRINT((ndo, " AID(%x) :%s: %s", ((uint16_t)(pbody.aid << 2 )) >> 2 ,
	    CAPABILITY_PRIVACY(pbody.capability_info) ? " PRIVACY " : "",
	    (pbody.status_code < NUM_STATUSES
		? status_text[pbody.status_code]
		: "n/a")));

	return ret;
}
