brcmf_cfg80211_del_pmksa(struct wiphy *wiphy, struct net_device *ndev,
			 struct cfg80211_pmksa *pmksa)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_pmksa *pmk = &cfg->pmk_list.pmk[0];
	s32 err;
	u32 npmk, i;

	brcmf_dbg(TRACE, "Enter\n");
	if (!check_vif_up(ifp->vif))
		return -EIO;

	brcmf_dbg(CONN, "del_pmksa - PMK bssid = %pM\n", &pmksa->bssid);

	npmk = le32_to_cpu(cfg->pmk_list.npmk);
	for (i = 0; i < npmk; i++)
		if (!memcmp(&pmksa->bssid, &pmk[i].bssid, ETH_ALEN))
			break;

	if ((npmk > 0) && (i < npmk)) {
		for (; i < (npmk - 1); i++) {
			memcpy(&pmk[i].bssid, &pmk[i + 1].bssid, ETH_ALEN);
			memcpy(&pmk[i].pmkid, &pmk[i + 1].pmkid,
			       WLAN_PMKID_LEN);
		}
		memset(&pmk[i], 0, sizeof(*pmk));
		cfg->pmk_list.npmk = cpu_to_le32(npmk - 1);
	} else {
		brcmf_err("Cache entry not found\n");
		return -EINVAL;
	}

	err = brcmf_update_pmklist(cfg, ifp);

	brcmf_dbg(TRACE, "Exit\n");
	return err;

}
