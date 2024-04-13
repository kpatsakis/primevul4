brcmf_cfg80211_set_pmksa(struct wiphy *wiphy, struct net_device *ndev,
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

	npmk = le32_to_cpu(cfg->pmk_list.npmk);
	for (i = 0; i < npmk; i++)
		if (!memcmp(pmksa->bssid, pmk[i].bssid, ETH_ALEN))
			break;
	if (i < BRCMF_MAXPMKID) {
		memcpy(pmk[i].bssid, pmksa->bssid, ETH_ALEN);
		memcpy(pmk[i].pmkid, pmksa->pmkid, WLAN_PMKID_LEN);
		if (i == npmk) {
			npmk++;
			cfg->pmk_list.npmk = cpu_to_le32(npmk);
		}
	} else {
		brcmf_err("Too many PMKSA entries cached %d\n", npmk);
		return -EINVAL;
	}

	brcmf_dbg(CONN, "set_pmksa - PMK bssid: %pM =\n", pmk[npmk].bssid);
	for (i = 0; i < WLAN_PMKID_LEN; i += 4)
		brcmf_dbg(CONN, "%02x %02x %02x %02x\n", pmk[npmk].pmkid[i],
			  pmk[npmk].pmkid[i + 1], pmk[npmk].pmkid[i + 2],
			  pmk[npmk].pmkid[i + 3]);

	err = brcmf_update_pmklist(cfg, ifp);

	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
