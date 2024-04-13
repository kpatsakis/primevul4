ctrl_header_print(netdissect_options *ndo, uint16_t fc, const u_char *p)
{
	switch (FC_SUBTYPE(fc)) {
	case CTRL_BAR:
		ND_PRINT((ndo, " RA:%s TA:%s CTL(%x) SEQ(%u) ",
		    etheraddr_string(ndo, ((const struct ctrl_bar_hdr_t *)p)->ra),
		    etheraddr_string(ndo, ((const struct ctrl_bar_hdr_t *)p)->ta),
		    EXTRACT_LE_16BITS(&(((const struct ctrl_bar_hdr_t *)p)->ctl)),
		    EXTRACT_LE_16BITS(&(((const struct ctrl_bar_hdr_t *)p)->seq))));
		break;
	case CTRL_BA:
		ND_PRINT((ndo, "RA:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_ba_hdr_t *)p)->ra)));
		break;
	case CTRL_PS_POLL:
		ND_PRINT((ndo, "BSSID:%s TA:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_ps_poll_hdr_t *)p)->bssid),
		    etheraddr_string(ndo, ((const struct ctrl_ps_poll_hdr_t *)p)->ta)));
		break;
	case CTRL_RTS:
		ND_PRINT((ndo, "RA:%s TA:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_rts_hdr_t *)p)->ra),
		    etheraddr_string(ndo, ((const struct ctrl_rts_hdr_t *)p)->ta)));
		break;
	case CTRL_CTS:
		ND_PRINT((ndo, "RA:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_cts_hdr_t *)p)->ra)));
		break;
	case CTRL_ACK:
		ND_PRINT((ndo, "RA:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_ack_hdr_t *)p)->ra)));
		break;
	case CTRL_CF_END:
		ND_PRINT((ndo, "RA:%s BSSID:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_end_hdr_t *)p)->ra),
		    etheraddr_string(ndo, ((const struct ctrl_end_hdr_t *)p)->bssid)));
		break;
	case CTRL_END_ACK:
		ND_PRINT((ndo, "RA:%s BSSID:%s ",
		    etheraddr_string(ndo, ((const struct ctrl_end_ack_hdr_t *)p)->ra),
		    etheraddr_string(ndo, ((const struct ctrl_end_ack_hdr_t *)p)->bssid)));
		break;
	default:
		/* We shouldn't get here - we should already have quit */
		break;
	}
}
