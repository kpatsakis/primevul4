static s32 brcmf_cfg80211_resume(struct wiphy *wiphy)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct net_device *ndev = cfg_to_ndev(cfg);
	struct brcmf_if *ifp = netdev_priv(ndev);

	brcmf_dbg(TRACE, "Enter\n");

	if (cfg->wowl.active) {
		brcmf_report_wowl_wakeind(wiphy, ifp);
		brcmf_fil_iovar_int_set(ifp, "wowl_clear", 0);
		brcmf_config_wowl_pattern(ifp, "clr", NULL, 0, NULL, 0);
		if (!brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_ARP_ND))
			brcmf_configure_arp_nd_offload(ifp, true);
		brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_PM,
				      cfg->wowl.pre_pmmode);
		cfg->wowl.active = false;
		if (cfg->wowl.nd_enabled) {
			brcmf_cfg80211_sched_scan_stop(cfg->wiphy, ifp->ndev);
			brcmf_fweh_unregister(cfg->pub, BRCMF_E_PFN_NET_FOUND);
			brcmf_fweh_register(cfg->pub, BRCMF_E_PFN_NET_FOUND,
					    brcmf_notify_sched_scan_results);
			cfg->wowl.nd_enabled = false;
		}
	}
	return 0;
}
