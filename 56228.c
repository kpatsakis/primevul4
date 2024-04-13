aodv_print(netdissect_options *ndo,
           const u_char *dat, u_int length, int is_ip6)
{
	uint8_t msg_type;

	/*
	 * The message type is the first byte; make sure we have it
	 * and then fetch it.
	 */
	ND_TCHECK(*dat);
	msg_type = *dat;
	ND_PRINT((ndo, " aodv"));

	switch (msg_type) {

	case AODV_RREQ:
		if (is_ip6)
			aodv_v6_rreq(ndo, dat, length);
		else
			aodv_rreq(ndo, dat, length);
		break;

	case AODV_RREP:
		if (is_ip6)
			aodv_v6_rrep(ndo, dat, length);
		else
			aodv_rrep(ndo, dat, length);
		break;

	case AODV_RERR:
		if (is_ip6)
			aodv_v6_rerr(ndo, dat, length);
		else
			aodv_rerr(ndo, dat, length);
		break;

	case AODV_RREP_ACK:
		ND_PRINT((ndo, " rrep-ack %u", length));
		break;

	case AODV_V6_DRAFT_01_RREQ:
		aodv_v6_draft_01_rreq(ndo, dat, length);
		break;

	case AODV_V6_DRAFT_01_RREP:
		aodv_v6_draft_01_rrep(ndo, dat, length);
		break;

	case AODV_V6_DRAFT_01_RERR:
		aodv_v6_draft_01_rerr(ndo, dat, length);
		break;

	case AODV_V6_DRAFT_01_RREP_ACK:
		ND_PRINT((ndo, " rrep-ack %u", length));
		break;

	default:
		ND_PRINT((ndo, " type %u %u", msg_type, length));
	}
	return;

trunc:
	ND_PRINT((ndo, " [|aodv]"));
}
