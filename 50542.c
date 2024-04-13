brcmf_cfg80211_reconfigure_wep(struct brcmf_if *ifp)
{
	s32 err;
	u8 key_idx;
	struct brcmf_wsec_key *key;
	s32 wsec;

	for (key_idx = 0; key_idx < BRCMF_MAX_DEFAULT_KEYS; key_idx++) {
		key = &ifp->vif->profile.key[key_idx];
		if ((key->algo == CRYPTO_ALGO_WEP1) ||
		    (key->algo == CRYPTO_ALGO_WEP128))
			break;
	}
	if (key_idx == BRCMF_MAX_DEFAULT_KEYS)
		return;

	err = send_key_to_dongle(ifp, key);
	if (err) {
		brcmf_err("Setting WEP key failed (%d)\n", err);
		return;
	}
	err = brcmf_fil_bsscfg_int_get(ifp, "wsec", &wsec);
	if (err) {
		brcmf_err("get wsec error (%d)\n", err);
		return;
	}
	wsec |= WEP_ENABLED;
	err = brcmf_fil_bsscfg_int_set(ifp, "wsec", wsec);
	if (err)
		brcmf_err("set wsec error (%d)\n", err);
}
