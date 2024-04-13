brcmf_cfg80211_set_rekey_data(struct wiphy *wiphy, struct net_device *ndev,
			      struct cfg80211_gtk_rekey_data *gtk)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_gtk_keyinfo_le gtk_le;
	int ret;

	brcmf_dbg(TRACE, "Enter, bssidx=%d\n", ifp->bsscfgidx);

	memcpy(gtk_le.kck, gtk->kck, sizeof(gtk_le.kck));
	memcpy(gtk_le.kek, gtk->kek, sizeof(gtk_le.kek));
	memcpy(gtk_le.replay_counter, gtk->replay_ctr,
	       sizeof(gtk_le.replay_counter));

	ret = brcmf_fil_iovar_data_set(ifp, "gtk_key_info", &gtk_le,
				       sizeof(gtk_le));
	if (ret < 0)
		brcmf_err("gtk_key_info iovar failed: ret=%d\n", ret);

	return ret;
}
