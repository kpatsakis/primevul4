atm_print(netdissect_options *ndo,
          u_int vpi, u_int vci, u_int traftype, const u_char *p, u_int length,
          u_int caplen)
{
	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "VPI:%u VCI:%u ", vpi, vci));

	if (vpi == 0) {
		switch (vci) {

		case VCI_PPC:
			sig_print(ndo, p);
			return;

		case VCI_BCC:
			ND_PRINT((ndo, "broadcast sig: "));
			return;

		case VCI_OAMF4SC: /* fall through */
		case VCI_OAMF4EC:
			oam_print(ndo, p, length, ATM_OAM_HEC);
			return;

		case VCI_METAC:
			ND_PRINT((ndo, "meta: "));
			return;

		case VCI_ILMIC:
			ND_PRINT((ndo, "ilmi: "));
			snmp_print(ndo, p, length);
			return;
		}
	}

	switch (traftype) {

	case ATM_LLC:
	default:
		/*
		 * Assumes traffic is LLC if unknown.
		 */
		atm_llc_print(ndo, p, length, caplen);
		break;

	case ATM_LANE:
		lane_print(ndo, p, length, caplen);
		break;
	}
}
