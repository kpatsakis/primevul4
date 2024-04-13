brcmf_cfg80211_add_key(struct wiphy *wiphy, struct net_device *ndev,
		       u8 key_idx, bool pairwise, const u8 *mac_addr,
		       struct key_params *params)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_wsec_key *key;
	s32 val;
	s32 wsec;
	s32 err;
	u8 keybuf[8];
	bool ext_key;

	brcmf_dbg(TRACE, "Enter\n");
	brcmf_dbg(CONN, "key index (%d)\n", key_idx);
	if (!check_vif_up(ifp->vif))
		return -EIO;

	if (key_idx >= BRCMF_MAX_DEFAULT_KEYS) {
		/* we ignore this key index in this case */
		brcmf_err("invalid key index (%d)\n", key_idx);
		return -EINVAL;
	}

	if (params->key_len == 0)
		return brcmf_cfg80211_del_key(wiphy, ndev, key_idx, pairwise,
					      mac_addr);

	if (params->key_len > sizeof(key->data)) {
		brcmf_err("Too long key length (%u)\n", params->key_len);
		return -EINVAL;
	}

	ext_key = false;
	if (mac_addr && (params->cipher != WLAN_CIPHER_SUITE_WEP40) &&
	    (params->cipher != WLAN_CIPHER_SUITE_WEP104)) {
		brcmf_dbg(TRACE, "Ext key, mac %pM", mac_addr);
		ext_key = true;
	}

	key = &ifp->vif->profile.key[key_idx];
	memset(key, 0, sizeof(*key));
	if ((ext_key) && (!is_multicast_ether_addr(mac_addr)))
		memcpy((char *)&key->ea, (void *)mac_addr, ETH_ALEN);
	key->len = params->key_len;
	key->index = key_idx;
	memcpy(key->data, params->key, key->len);
	if (!ext_key)
		key->flags = BRCMF_PRIMARY_KEY;

	switch (params->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
		key->algo = CRYPTO_ALGO_WEP1;
		val = WEP_ENABLED;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_WEP40\n");
		break;
	case WLAN_CIPHER_SUITE_WEP104:
		key->algo = CRYPTO_ALGO_WEP128;
		val = WEP_ENABLED;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_WEP104\n");
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		if (!brcmf_is_apmode(ifp->vif)) {
			brcmf_dbg(CONN, "Swapping RX/TX MIC key\n");
			memcpy(keybuf, &key->data[24], sizeof(keybuf));
			memcpy(&key->data[24], &key->data[16], sizeof(keybuf));
			memcpy(&key->data[16], keybuf, sizeof(keybuf));
		}
		key->algo = CRYPTO_ALGO_TKIP;
		val = TKIP_ENABLED;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_TKIP\n");
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
		key->algo = CRYPTO_ALGO_AES_CCM;
		val = AES_ENABLED;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_AES_CMAC\n");
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		key->algo = CRYPTO_ALGO_AES_CCM;
		val = AES_ENABLED;
		brcmf_dbg(CONN, "WLAN_CIPHER_SUITE_CCMP\n");
		break;
	default:
		brcmf_err("Invalid cipher (0x%x)\n", params->cipher);
		err = -EINVAL;
		goto done;
	}

	err = send_key_to_dongle(ifp, key);
	if (ext_key || err)
		goto done;

	err = brcmf_fil_bsscfg_int_get(ifp, "wsec", &wsec);
	if (err) {
		brcmf_err("get wsec error (%d)\n", err);
		goto done;
	}
	wsec |= val;
	err = brcmf_fil_bsscfg_int_set(ifp, "wsec", wsec);
	if (err) {
		brcmf_err("set wsec error (%d)\n", err);
		goto done;
	}

done:
	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
