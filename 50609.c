static void brcmf_report_wowl_wakeind(struct wiphy *wiphy, struct brcmf_if *ifp)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_wowl_wakeind_le wake_ind_le;
	struct cfg80211_wowlan_wakeup wakeup_data;
	struct cfg80211_wowlan_wakeup *wakeup;
	u32 wakeind;
	s32 err;
	int timeout;

	err = brcmf_fil_iovar_data_get(ifp, "wowl_wakeind", &wake_ind_le,
				       sizeof(wake_ind_le));
	if (err) {
		brcmf_err("Get wowl_wakeind failed, err = %d\n", err);
		return;
	}

	wakeind = le32_to_cpu(wake_ind_le.ucode_wakeind);
	if (wakeind & (BRCMF_WOWL_MAGIC | BRCMF_WOWL_DIS | BRCMF_WOWL_BCN |
		       BRCMF_WOWL_RETR | BRCMF_WOWL_NET |
		       BRCMF_WOWL_PFN_FOUND)) {
		wakeup = &wakeup_data;
		memset(&wakeup_data, 0, sizeof(wakeup_data));
		wakeup_data.pattern_idx = -1;

		if (wakeind & BRCMF_WOWL_MAGIC) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_MAGIC\n");
			wakeup_data.magic_pkt = true;
		}
		if (wakeind & BRCMF_WOWL_DIS) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_DIS\n");
			wakeup_data.disconnect = true;
		}
		if (wakeind & BRCMF_WOWL_BCN) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_BCN\n");
			wakeup_data.disconnect = true;
		}
		if (wakeind & BRCMF_WOWL_RETR) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_RETR\n");
			wakeup_data.disconnect = true;
		}
		if (wakeind & BRCMF_WOWL_NET) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_NET\n");
			/* For now always map to pattern 0, no API to get
			 * correct information available at the moment.
			 */
			wakeup_data.pattern_idx = 0;
		}
		if (wakeind & BRCMF_WOWL_PFN_FOUND) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_PFN_FOUND\n");
			timeout = wait_event_timeout(cfg->wowl.nd_data_wait,
				cfg->wowl.nd_data_completed,
				BRCMF_ND_INFO_TIMEOUT);
			if (!timeout)
				brcmf_err("No result for wowl net detect\n");
			else
				wakeup_data.net_detect = cfg->wowl.nd_info;
		}
		if (wakeind & BRCMF_WOWL_GTK_FAILURE) {
			brcmf_dbg(INFO, "WOWL Wake indicator: BRCMF_WOWL_GTK_FAILURE\n");
			wakeup_data.gtk_rekey_failure = true;
		}
	} else {
		wakeup = NULL;
	}
	cfg80211_report_wowlan_wakeup(&ifp->vif->wdev, wakeup, GFP_KERNEL);
}
