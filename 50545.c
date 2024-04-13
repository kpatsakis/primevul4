brcmf_cfg80211_sched_scan_start(struct wiphy *wiphy,
				struct net_device *ndev,
				struct cfg80211_sched_scan_request *request)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_info *cfg = wiphy_priv(wiphy);
	struct brcmf_pno_net_param_le pfn;
	int i;
	int ret = 0;

	brcmf_dbg(SCAN, "Enter n_match_sets:%d n_ssids:%d\n",
		  request->n_match_sets, request->n_ssids);
	if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
		brcmf_err("Scanning already: status (%lu)\n", cfg->scan_status);
		return -EAGAIN;
	}
	if (test_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status)) {
		brcmf_err("Scanning suppressed: status (%lu)\n",
			  cfg->scan_status);
		return -EAGAIN;
	}

	if (!request->n_ssids || !request->n_match_sets) {
		brcmf_dbg(SCAN, "Invalid sched scan req!! n_ssids:%d\n",
			  request->n_ssids);
		return -EINVAL;
	}

	if (request->n_ssids > 0) {
		for (i = 0; i < request->n_ssids; i++) {
			/* Active scan req for ssids */
			brcmf_dbg(SCAN, ">>> Active scan req for ssid (%s)\n",
				  request->ssids[i].ssid);

			/* match_set ssids is a supert set of n_ssid list,
			 * so we need not add these set separately.
			 */
		}
	}

	if (request->n_match_sets > 0) {
		/* clean up everything */
		ret = brcmf_dev_pno_clean(ndev);
		if  (ret < 0) {
			brcmf_err("failed error=%d\n", ret);
			return ret;
		}

		/* configure pno */
		if (brcmf_dev_pno_config(ifp, request))
			return -EINVAL;

		/* configure each match set */
		for (i = 0; i < request->n_match_sets; i++) {
			struct cfg80211_ssid *ssid;
			u32 ssid_len;

			ssid = &request->match_sets[i].ssid;
			ssid_len = ssid->ssid_len;

			if (!ssid_len) {
				brcmf_err("skip broadcast ssid\n");
				continue;
			}
			pfn.auth = cpu_to_le32(WLAN_AUTH_OPEN);
			pfn.wpa_auth = cpu_to_le32(BRCMF_PNO_WPA_AUTH_ANY);
			pfn.wsec = cpu_to_le32(0);
			pfn.infra = cpu_to_le32(1);
			pfn.flags = cpu_to_le32(1 << BRCMF_PNO_HIDDEN_BIT);
			pfn.ssid.SSID_len = cpu_to_le32(ssid_len);
			memcpy(pfn.ssid.SSID, ssid->ssid, ssid_len);
			ret = brcmf_fil_iovar_data_set(ifp, "pfn_add", &pfn,
						       sizeof(pfn));
			brcmf_dbg(SCAN, ">>> PNO filter %s for ssid (%s)\n",
				  ret == 0 ? "set" : "failed", ssid->ssid);
		}
		/* Enable the PNO */
		if (brcmf_fil_iovar_int_set(ifp, "pfn", 1) < 0) {
			brcmf_err("PNO enable failed!! ret=%d\n", ret);
			return -EINVAL;
		}
	} else {
		return -EINVAL;
	}

	return 0;
}
