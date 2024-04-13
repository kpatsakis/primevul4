static s32 brcmf_cfg80211_suspend(struct wiphy *wiphy,
				  struct cfg80211_wowlan *wowl)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct net_device *ndev = cfg_to_ndev(cfg);
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_vif *vif;

	brcmf_dbg(TRACE, "Enter\n");

	/* if the primary net_device is not READY there is nothing
	 * we can do but pray resume goes smoothly.
	 */
	if (!check_vif_up(ifp->vif))
		goto exit;

	/* Stop scheduled scan */
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_PNO))
		brcmf_cfg80211_sched_scan_stop(wiphy, ndev);

	/* end any scanning */
	if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status))
		brcmf_abort_scanning(cfg);

	if (wowl == NULL) {
		brcmf_bus_wowl_config(cfg->pub->bus_if, false);
		list_for_each_entry(vif, &cfg->vif_list, list) {
			if (!test_bit(BRCMF_VIF_STATUS_READY, &vif->sme_state))
				continue;
			/* While going to suspend if associated with AP
			 * disassociate from AP to save power while system is
			 * in suspended state
			 */
			brcmf_link_down(vif, WLAN_REASON_UNSPECIFIED);
			/* Make sure WPA_Supplicant receives all the event
			 * generated due to DISASSOC call to the fw to keep
			 * the state fw and WPA_Supplicant state consistent
			 */
			brcmf_delay(500);
		}
		/* Configure MPC */
		brcmf_set_mpc(ifp, 1);

	} else {
		/* Configure WOWL paramaters */
		brcmf_configure_wowl(cfg, ifp, wowl);
	}

exit:
	brcmf_dbg(TRACE, "Exit\n");
	/* clear any scanning activity */
	cfg->scan_status = 0;
	return 0;
}
