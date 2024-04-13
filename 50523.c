brcmf_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev,
		       u8 key_idx, bool pairwise, const u8 *mac_addr)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_wsec_key *key;
	s32 err;

	brcmf_dbg(TRACE, "Enter\n");
	brcmf_dbg(CONN, "key index (%d)\n", key_idx);

	if (!check_vif_up(ifp->vif))
		return -EIO;

	if (key_idx >= BRCMF_MAX_DEFAULT_KEYS) {
		/* we ignore this key index in this case */
		return -EINVAL;
	}

	key = &ifp->vif->profile.key[key_idx];

	if (key->algo == CRYPTO_ALGO_OFF) {
		brcmf_dbg(CONN, "Ignore clearing of (never configured) key\n");
		return -EINVAL;
	}

	memset(key, 0, sizeof(*key));
	key->index = (u32)key_idx;
	key->flags = BRCMF_PRIMARY_KEY;

	/* Clear the key/index */
	err = send_key_to_dongle(ifp, key);

	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
