static void brcmf_link_down(struct brcmf_cfg80211_vif *vif, u16 reason)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(vif->wdev.wiphy);
	s32 err = 0;

	brcmf_dbg(TRACE, "Enter\n");

	if (test_and_clear_bit(BRCMF_VIF_STATUS_CONNECTED, &vif->sme_state)) {
		brcmf_dbg(INFO, "Call WLC_DISASSOC to stop excess roaming\n ");
		err = brcmf_fil_cmd_data_set(vif->ifp,
					     BRCMF_C_DISASSOC, NULL, 0);
		if (err) {
			brcmf_err("WLC_DISASSOC failed (%d)\n", err);
		}
		if ((vif->wdev.iftype == NL80211_IFTYPE_STATION) ||
		    (vif->wdev.iftype == NL80211_IFTYPE_P2P_CLIENT))
			cfg80211_disconnected(vif->wdev.netdev, reason, NULL, 0,
					      true, GFP_KERNEL);
	}
	clear_bit(BRCMF_VIF_STATUS_CONNECTING, &vif->sme_state);
	clear_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status);
	brcmf_btcoex_set_mode(vif, BRCMF_BTCOEX_ENABLED, 0);
	brcmf_dbg(TRACE, "Exit\n");
}
