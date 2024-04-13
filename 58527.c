static int brcmf_cfg80211_del_pmk(struct wiphy *wiphy, struct net_device *dev,
				  const u8 *aa)
{
	struct brcmf_if *ifp;

	brcmf_dbg(TRACE, "enter\n");
	ifp = netdev_priv(dev);
	if (WARN_ON(ifp->vif->profile.use_fwsup != BRCMF_PROFILE_FWSUP_1X))
		return -EINVAL;

	return brcmf_set_pmk(ifp, NULL, 0);
}
