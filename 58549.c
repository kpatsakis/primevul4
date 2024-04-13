static bool brcmf_is_linkup(struct brcmf_cfg80211_vif *vif,
			    const struct brcmf_event_msg *e)
{
	u32 event = e->event_code;
	u32 status = e->status;

	if (vif->profile.use_fwsup == BRCMF_PROFILE_FWSUP_PSK &&
	    event == BRCMF_E_PSK_SUP &&
	    status == BRCMF_E_STATUS_FWSUP_COMPLETED)
		set_bit(BRCMF_VIF_STATUS_EAP_SUCCESS, &vif->sme_state);
	if (event == BRCMF_E_SET_SSID && status == BRCMF_E_STATUS_SUCCESS) {
		brcmf_dbg(CONN, "Processing set ssid\n");
		memcpy(vif->profile.bssid, e->addr, ETH_ALEN);
		if (vif->profile.use_fwsup != BRCMF_PROFILE_FWSUP_PSK)
			return true;

		set_bit(BRCMF_VIF_STATUS_ASSOC_SUCCESS, &vif->sme_state);
	}

	if (test_bit(BRCMF_VIF_STATUS_EAP_SUCCESS, &vif->sme_state) &&
	    test_bit(BRCMF_VIF_STATUS_ASSOC_SUCCESS, &vif->sme_state)) {
		clear_bit(BRCMF_VIF_STATUS_EAP_SUCCESS, &vif->sme_state);
		clear_bit(BRCMF_VIF_STATUS_ASSOC_SUCCESS, &vif->sme_state);
		return true;
	}
	return false;
}
