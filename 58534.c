brcmf_cfg80211_sched_scan_start(struct wiphy *wiphy,
				struct net_device *ndev,
				struct cfg80211_sched_scan_request *req)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_info *cfg = wiphy_priv(wiphy);

	brcmf_dbg(SCAN, "Enter: n_match_sets=%d n_ssids=%d\n",
		  req->n_match_sets, req->n_ssids);

	if (test_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status)) {
		brcmf_err("Scanning suppressed: status=%lu\n",
			  cfg->scan_status);
		return -EAGAIN;
	}

	if (req->n_match_sets <= 0) {
		brcmf_dbg(SCAN, "invalid number of matchsets specified: %d\n",
			  req->n_match_sets);
		return -EINVAL;
	}

	return brcmf_pno_start_sched_scan(ifp, req);
}
