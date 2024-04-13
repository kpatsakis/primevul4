ctrl_body_print(netdissect_options *ndo,
                uint16_t fc, const u_char *p)
{
	ND_PRINT((ndo, "%s", tok2str(ctrl_str, "Unknown Ctrl Subtype", FC_SUBTYPE(fc))));
	switch (FC_SUBTYPE(fc)) {
	case CTRL_CONTROL_WRAPPER:
		/* XXX - requires special handling */
		break;
	case CTRL_BAR:
		if (!ND_TTEST2(*p, CTRL_BAR_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s TA:%s CTL(%x) SEQ(%u) ",
			    etheraddr_string(ndo, ((const struct ctrl_bar_hdr_t *)p)->ra),
			    etheraddr_string(ndo, ((const struct ctrl_bar_hdr_t *)p)->ta),
			    EXTRACT_LE_16BITS(&(((const struct ctrl_bar_hdr_t *)p)->ctl)),
			    EXTRACT_LE_16BITS(&(((const struct ctrl_bar_hdr_t *)p)->seq))));
		break;
	case CTRL_BA:
		if (!ND_TTEST2(*p, CTRL_BA_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_ba_hdr_t *)p)->ra)));
		break;
	case CTRL_PS_POLL:
		if (!ND_TTEST2(*p, CTRL_PS_POLL_HDRLEN))
			return 0;
		ND_PRINT((ndo, " AID(%x)",
		    EXTRACT_LE_16BITS(&(((const struct ctrl_ps_poll_hdr_t *)p)->aid))));
		break;
	case CTRL_RTS:
		if (!ND_TTEST2(*p, CTRL_RTS_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " TA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_rts_hdr_t *)p)->ta)));
		break;
	case CTRL_CTS:
		if (!ND_TTEST2(*p, CTRL_CTS_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_cts_hdr_t *)p)->ra)));
		break;
	case CTRL_ACK:
		if (!ND_TTEST2(*p, CTRL_ACK_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_ack_hdr_t *)p)->ra)));
		break;
	case CTRL_CF_END:
		if (!ND_TTEST2(*p, CTRL_END_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_end_hdr_t *)p)->ra)));
		break;
	case CTRL_END_ACK:
		if (!ND_TTEST2(*p, CTRL_END_ACK_HDRLEN))
			return 0;
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, " RA:%s ",
			    etheraddr_string(ndo, ((const struct ctrl_end_ack_hdr_t *)p)->ra)));
		break;
	}
	return 1;
}
