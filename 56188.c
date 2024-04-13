handle_disassoc(netdissect_options *ndo,
                const u_char *p, u_int length)
{
	struct mgmt_body_t  pbody;

	memset(&pbody, 0, sizeof(pbody));

	if (!ND_TTEST2(*p, IEEE802_11_REASON_LEN))
		return 0;
	if (length < IEEE802_11_REASON_LEN)
		return 0;
	pbody.reason_code = EXTRACT_LE_16BITS(p);

	ND_PRINT((ndo, ": %s",
	    (pbody.reason_code < NUM_REASONS)
		? reason_text[pbody.reason_code]
		: "Reserved"));

	return 1;
}
