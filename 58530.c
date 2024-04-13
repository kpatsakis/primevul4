brcmf_cfg80211_escan(struct wiphy *wiphy, struct brcmf_cfg80211_vif *vif,
		     struct cfg80211_scan_request *request,
		     struct cfg80211_ssid *this_ssid)
{
	struct brcmf_if *ifp = vif->ifp;
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct cfg80211_ssid *ssids;
	u32 passive_scan;
	bool escan_req;
	bool spec_scan;
	s32 err;
	struct brcmf_ssid_le ssid_le;
	u32 SSID_len;

	brcmf_dbg(SCAN, "START ESCAN\n");

	if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
		brcmf_err("Scanning already: status (%lu)\n", cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_SCAN_STATUS_ABORT, &cfg->scan_status)) {
		brcmf_err("Scanning being aborted: status (%lu)\n",
			  cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status)) {
		brcmf_err("Scanning suppressed: status (%lu)\n",
			  cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_VIF_STATUS_CONNECTING, &ifp->vif->sme_state)) {
		brcmf_err("Connecting: status (%lu)\n", ifp->vif->sme_state);
		return -EAGAIN;
	}

	/* If scan req comes for p2p0, send it over primary I/F */
	if (vif == cfg->p2p.bss_idx[P2PAPI_BSSCFG_DEVICE].vif)
		vif = cfg->p2p.bss_idx[P2PAPI_BSSCFG_PRIMARY].vif;

	escan_req = false;
	if (request) {
		/* scan bss */
		ssids = request->ssids;
		escan_req = true;
	} else {
		/* scan in ibss */
		/* we don't do escan in ibss */
		ssids = this_ssid;
	}

	cfg->scan_request = request;
	set_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
	if (escan_req) {
		cfg->escan_info.run = brcmf_run_escan;
		err = brcmf_p2p_scan_prep(wiphy, request, vif);
		if (err)
			goto scan_out;

		err = brcmf_do_escan(vif->ifp, request);
		if (err)
			goto scan_out;
	} else {
		brcmf_dbg(SCAN, "ssid \"%s\", ssid_len (%d)\n",
			  ssids->ssid, ssids->ssid_len);
		memset(&ssid_le, 0, sizeof(ssid_le));
		SSID_len = min_t(u8, sizeof(ssid_le.SSID), ssids->ssid_len);
		ssid_le.SSID_len = cpu_to_le32(0);
		spec_scan = false;
		if (SSID_len) {
			memcpy(ssid_le.SSID, ssids->ssid, SSID_len);
			ssid_le.SSID_len = cpu_to_le32(SSID_len);
			spec_scan = true;
		} else
			brcmf_dbg(SCAN, "Broadcast scan\n");

		passive_scan = cfg->active_scan ? 0 : 1;
		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_PASSIVE_SCAN,
					    passive_scan);
		if (err) {
			brcmf_err("WLC_SET_PASSIVE_SCAN error (%d)\n", err);
			goto scan_out;
		}
		brcmf_scan_config_mpc(ifp, 0);
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SCAN, &ssid_le,
					     sizeof(ssid_le));
		if (err) {
			if (err == -EBUSY)
				brcmf_dbg(INFO, "BUSY: scan for \"%s\" canceled\n",
					  ssid_le.SSID);
			else
				brcmf_err("WLC_SCAN error (%d)\n", err);

			brcmf_scan_config_mpc(ifp, 1);
			goto scan_out;
		}
	}

	/* Arm scan timeout timer */
	mod_timer(&cfg->escan_timeout, jiffies +
			BRCMF_ESCAN_TIMER_INTERVAL_MS * HZ / 1000);

	return 0;

scan_out:
	clear_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
	cfg->scan_request = NULL;
	return err;
}
