handle_action(netdissect_options *ndo,
              const uint8_t *src, const u_char *p, u_int length)
{
	if (!ND_TTEST2(*p, 2))
		return 0;
	if (length < 2)
		return 0;
	if (ndo->ndo_eflag) {
		ND_PRINT((ndo, ": "));
	} else {
		ND_PRINT((ndo, " (%s): ", etheraddr_string(ndo, src)));
	}
	switch (p[0]) {
	case 0: ND_PRINT((ndo, "Spectrum Management Act#%d", p[1])); break;
	case 1: ND_PRINT((ndo, "QoS Act#%d", p[1])); break;
	case 2: ND_PRINT((ndo, "DLS Act#%d", p[1])); break;
	case 3: ND_PRINT((ndo, "BA ")); PRINT_BA_ACTION(p[1]); break;
	case 7: ND_PRINT((ndo, "HT ")); PRINT_HT_ACTION(p[1]); break;
	case 13: ND_PRINT((ndo, "MeshAction ")); PRINT_MESH_ACTION(p[1]); break;
	case 14:
		ND_PRINT((ndo, "MultiohopAction "));
		PRINT_MULTIHOP_ACTION(p[1]); break;
	case 15:
		ND_PRINT((ndo, "SelfprotectAction "));
		PRINT_SELFPROT_ACTION(p[1]); break;
	case 127: ND_PRINT((ndo, "Vendor Act#%d", p[1])); break;
	default:
		ND_PRINT((ndo, "Reserved(%d) Act#%d", p[0], p[1]));
		break;
	}
	return 1;
}
