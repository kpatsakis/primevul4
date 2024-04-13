static int brcmf_cfg80211_sched_scan_stop(struct wiphy *wiphy,
					  struct net_device *ndev, u64 reqid)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_if *ifp = netdev_priv(ndev);

	brcmf_dbg(SCAN, "enter\n");
	brcmf_pno_stop_sched_scan(ifp, reqid);
	if (cfg->int_escan_map)
		brcmf_notify_escan_complete(cfg, ifp, true, true);
	return 0;
}
