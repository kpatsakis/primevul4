brcmf_cfg80211_get_key(struct wiphy *wiphy, struct net_device *ndev, u8 key_idx,
		       bool pairwise, const u8 *mac_addr, void *cookie,
		       void (*callback)(void *cookie,
					struct key_params *params))
{
	struct key_params params;
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_profile *profile = &ifp->vif->profile;
	struct brcmf_cfg80211_security *sec;
	s32 wsec;
	s32 err = 0;

	brcmf_dbg(TRACE, "Enter\n");
	brcmf_dbg(CONN, "key index (%d)\n", key_idx);
	if (!check_vif_up(ifp->vif))
		return -EIO;

	memset(&params, 0, sizeof(params));

	err = brcmf_fil_bsscfg_int_get(ifp, "wsec", &wsec);
	if (err) {
		brcmf_err("WLC_GET_WSEC error (%d)\n", err);
		/* Ignore this error, may happen during DISASSOC */
		err = -EAGAIN;
		goto done;
	}
	if (wsec & WEP_ENABLED) {
		sec = &profile->sec;
		if (sec->cipher_pairwise & WLAN_CIPHER_SUITE_WEP40) {
			params.cipher = WLAN_CIPHER_SUITE_WEP40;
			brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_WEP40\n");
		} else if (sec->cipher_pairwise & WLAN_CIPHER_SUITE_WEP104) {
			params.cipher = WLAN_CIPHER_SUITE_WEP104;
			brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_WEP104\n");
		}
	} else if (wsec & TKIP_ENABLED) {
		params.cipher = WLAN_CIPHER_SUITE_TKIP;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_TKIP\n");
	} else if (wsec & AES_ENABLED) {
		params.cipher = WLAN_CIPHER_SUITE_AES_CMAC;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_AES_CMAC\n");
	} else  {
		brcmf_err("Invalid algo (0x%x)\n", wsec);
		err = -EINVAL;
		goto done;
	}
	callback(cookie, &params);

done:
	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
