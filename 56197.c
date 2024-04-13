mgmt_body_print(netdissect_options *ndo,
                uint16_t fc, const uint8_t *src, const u_char *p, u_int length)
{
	ND_PRINT((ndo, "%s", tok2str(st_str, "Unhandled Management subtype(%x)", FC_SUBTYPE(fc))));

	/* There may be a problem w/ AP not having this bit set */
	if (FC_PROTECTED(fc))
		return wep_print(ndo, p);
	switch (FC_SUBTYPE(fc)) {
	case ST_ASSOC_REQUEST:
		return handle_assoc_request(ndo, p, length);
	case ST_ASSOC_RESPONSE:
		return handle_assoc_response(ndo, p, length);
	case ST_REASSOC_REQUEST:
		return handle_reassoc_request(ndo, p, length);
	case ST_REASSOC_RESPONSE:
		return handle_reassoc_response(ndo, p, length);
	case ST_PROBE_REQUEST:
		return handle_probe_request(ndo, p, length);
	case ST_PROBE_RESPONSE:
		return handle_probe_response(ndo, p, length);
	case ST_BEACON:
		return handle_beacon(ndo, p, length);
	case ST_ATIM:
		return handle_atim();
	case ST_DISASSOC:
		return handle_disassoc(ndo, p, length);
	case ST_AUTH:
		return handle_auth(ndo, p, length);
	case ST_DEAUTH:
		return handle_deauth(ndo, src, p, length);
	case ST_ACTION:
		return handle_action(ndo, src, p, length);
	default:
		return 1;
	}
}
