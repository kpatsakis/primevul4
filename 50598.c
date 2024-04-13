static u16 brcmf_map_fw_linkdown_reason(const struct brcmf_event_msg *e)
{
	u16 reason;

	switch (e->event_code) {
	case BRCMF_E_DEAUTH:
	case BRCMF_E_DEAUTH_IND:
	case BRCMF_E_DISASSOC_IND:
		reason = e->reason;
		break;
	case BRCMF_E_LINK:
	default:
		reason = 0;
		break;
	}
	return reason;
}
