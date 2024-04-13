brcmf_cfg80211_change_station(struct wiphy *wiphy, struct net_device *ndev,
			      const u8 *mac, struct station_parameters *params)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	s32 err;

	brcmf_dbg(TRACE, "Enter, MAC %pM, mask 0x%04x set 0x%04x\n", mac,
		  params->sta_flags_mask, params->sta_flags_set);

	/* Ignore all 00 MAC */
	if (is_zero_ether_addr(mac))
		return 0;

	if (!(params->sta_flags_mask & BIT(NL80211_STA_FLAG_AUTHORIZED)))
		return 0;

	if (params->sta_flags_set & BIT(NL80211_STA_FLAG_AUTHORIZED))
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_SCB_AUTHORIZE,
					     (void *)mac, ETH_ALEN);
	else
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_SCB_DEAUTHORIZE,
					     (void *)mac, ETH_ALEN);
	if (err < 0)
		brcmf_err("Setting SCB (de-)authorize failed, %d\n", err);

	return err;
}
