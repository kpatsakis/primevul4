handle_deauth(netdissect_options *ndo,
              const uint8_t *src, const u_char *p, u_int length)
{
	struct mgmt_body_t  pbody;
	const char *reason = NULL;

	memset(&pbody, 0, sizeof(pbody));

	if (!ND_TTEST2(*p, IEEE802_11_REASON_LEN))
		return 0;
	if (length < IEEE802_11_REASON_LEN)
		return 0;
	pbody.reason_code = EXTRACT_LE_16BITS(p);

	reason = (pbody.reason_code < NUM_REASONS)
			? reason_text[pbody.reason_code]
			: "Reserved";

	if (ndo->ndo_eflag) {
		ND_PRINT((ndo, ": %s", reason));
	} else {
		ND_PRINT((ndo, " (%s): %s", etheraddr_string(ndo, src), reason));
	}
	return 1;
}
