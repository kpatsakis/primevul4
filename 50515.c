brcmf_cfg80211_change_beacon(struct wiphy *wiphy, struct net_device *ndev,
			     struct cfg80211_beacon_data *info)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	s32 err;

	brcmf_dbg(TRACE, "Enter\n");

	err = brcmf_config_ap_mgmt_ie(ifp->vif, info);

	return err;
}
