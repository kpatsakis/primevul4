brcmf_cfg80211_config_default_mgmt_key(struct wiphy *wiphy,
				       struct net_device *ndev, u8 key_idx)
{
	struct brcmf_if *ifp = netdev_priv(ndev);

	brcmf_dbg(TRACE, "Enter key_idx %d\n", key_idx);

	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MFP))
		return 0;

	brcmf_dbg(INFO, "Not supported\n");

	return -EOPNOTSUPP;
}
