static int brcmf_cfg80211_sched_scan_stop(struct wiphy *wiphy,
					  struct net_device *ndev)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);

	brcmf_dbg(SCAN, "enter\n");
	brcmf_dev_pno_clean(ndev);
	if (cfg->sched_escan)
		brcmf_notify_escan_complete(cfg, netdev_priv(ndev), true, true);
	return 0;
}
