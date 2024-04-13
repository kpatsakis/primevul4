brcmf_cfg80211_dump_station(struct wiphy *wiphy, struct net_device *ndev,
			    int idx, u8 *mac, struct station_info *sinfo)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_if *ifp = netdev_priv(ndev);
	s32 err;

	brcmf_dbg(TRACE, "Enter, idx %d\n", idx);

	if (idx == 0) {
		cfg->assoclist.count = cpu_to_le32(BRCMF_MAX_ASSOCLIST);
		err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_ASSOCLIST,
					     &cfg->assoclist,
					     sizeof(cfg->assoclist));
		if (err) {
			brcmf_err("BRCMF_C_GET_ASSOCLIST unsupported, err=%d\n",
				  err);
			cfg->assoclist.count = 0;
			return -EOPNOTSUPP;
		}
	}
	if (idx < le32_to_cpu(cfg->assoclist.count)) {
		memcpy(mac, cfg->assoclist.mac[idx], ETH_ALEN);
		return brcmf_cfg80211_get_station(wiphy, ndev, mac, sinfo);
	}
	return -ENOENT;
}
