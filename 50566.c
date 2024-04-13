static int brcmf_convert_nl80211_tdls_oper(enum nl80211_tdls_operation oper)
{
	int ret;

	switch (oper) {
	case NL80211_TDLS_DISCOVERY_REQ:
		ret = BRCMF_TDLS_MANUAL_EP_DISCOVERY;
		break;
	case NL80211_TDLS_SETUP:
		ret = BRCMF_TDLS_MANUAL_EP_CREATE;
		break;
	case NL80211_TDLS_TEARDOWN:
		ret = BRCMF_TDLS_MANUAL_EP_DELETE;
		break;
	default:
		brcmf_err("unsupported operation: %d\n", oper);
		ret = -EOPNOTSUPP;
	}
	return ret;
}
