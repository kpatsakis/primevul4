brcmf_cfg80211_leave_ibss(struct wiphy *wiphy, struct net_device *ndev)
{
	struct brcmf_if *ifp = netdev_priv(ndev);

	brcmf_dbg(TRACE, "Enter\n");
	if (!check_vif_up(ifp->vif)) {
		/* When driver is being unloaded, it can end up here. If an
		 * error is returned then later on a debug trace in the wireless
		 * core module will be printed. To avoid this 0 is returned.
		 */
		return 0;
	}

	brcmf_link_down(ifp->vif, WLAN_REASON_DEAUTH_LEAVING);
	brcmf_net_setcarrier(ifp, false);

	brcmf_dbg(TRACE, "Exit\n");

	return 0;
}
