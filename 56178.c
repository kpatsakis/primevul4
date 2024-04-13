extract_header_length(netdissect_options *ndo,
                      uint16_t fc)
{
	int len;

	switch (FC_TYPE(fc)) {
	case T_MGMT:
		return MGMT_HDRLEN;
	case T_CTRL:
		switch (FC_SUBTYPE(fc)) {
		case CTRL_CONTROL_WRAPPER:
			return CTRL_CONTROL_WRAPPER_HDRLEN;
		case CTRL_BAR:
			return CTRL_BAR_HDRLEN;
		case CTRL_BA:
			return CTRL_BA_HDRLEN;
		case CTRL_PS_POLL:
			return CTRL_PS_POLL_HDRLEN;
		case CTRL_RTS:
			return CTRL_RTS_HDRLEN;
		case CTRL_CTS:
			return CTRL_CTS_HDRLEN;
		case CTRL_ACK:
			return CTRL_ACK_HDRLEN;
		case CTRL_CF_END:
			return CTRL_END_HDRLEN;
		case CTRL_END_ACK:
			return CTRL_END_ACK_HDRLEN;
		default:
			ND_PRINT((ndo, "unknown 802.11 ctrl frame subtype (%d)", FC_SUBTYPE(fc)));
			return 0;
		}
	case T_DATA:
		len = (FC_TO_DS(fc) && FC_FROM_DS(fc)) ? 30 : 24;
		if (DATA_FRAME_IS_QOS(FC_SUBTYPE(fc)))
			len += 2;
		return len;
	default:
		ND_PRINT((ndo, "unknown 802.11 frame type (%d)", FC_TYPE(fc)));
		return 0;
	}
}
