struct brcmf_cfg80211_info *brcmf_cfg80211_attach(struct brcmf_pub *drvr,
						  struct device *busdev,
						  bool p2pdev_forced)
{
	struct net_device *ndev = brcmf_get_ifp(drvr, 0)->ndev;
	struct brcmf_cfg80211_info *cfg;
	struct wiphy *wiphy;
	struct cfg80211_ops *ops;
	struct brcmf_cfg80211_vif *vif;
	struct brcmf_if *ifp;
	s32 err = 0;
	s32 io_type;
	u16 *cap = NULL;

	if (!ndev) {
		brcmf_err("ndev is invalid\n");
		return NULL;
	}

	ops = kmemdup(&brcmf_cfg80211_ops, sizeof(*ops), GFP_KERNEL);
	if (!ops)
		return NULL;

	ifp = netdev_priv(ndev);
#ifdef CONFIG_PM
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_GTK))
		ops->set_rekey_data = brcmf_cfg80211_set_rekey_data;
#endif
	wiphy = wiphy_new(ops, sizeof(struct brcmf_cfg80211_info));
	if (!wiphy) {
		brcmf_err("Could not allocate wiphy device\n");
		return NULL;
	}
	memcpy(wiphy->perm_addr, drvr->mac, ETH_ALEN);
	set_wiphy_dev(wiphy, busdev);

	cfg = wiphy_priv(wiphy);
	cfg->wiphy = wiphy;
	cfg->ops = ops;
	cfg->pub = drvr;
	init_vif_event(&cfg->vif_event);
	INIT_LIST_HEAD(&cfg->vif_list);

	vif = brcmf_alloc_vif(cfg, NL80211_IFTYPE_STATION);
	if (IS_ERR(vif))
		goto wiphy_out;

	vif->ifp = ifp;
	vif->wdev.netdev = ndev;
	ndev->ieee80211_ptr = &vif->wdev;
	SET_NETDEV_DEV(ndev, wiphy_dev(cfg->wiphy));

	err = wl_init_priv(cfg);
	if (err) {
		brcmf_err("Failed to init iwm_priv (%d)\n", err);
		brcmf_free_vif(vif);
		goto wiphy_out;
	}
	ifp->vif = vif;

	/* determine d11 io type before wiphy setup */
	err = brcmf_fil_cmd_int_get(ifp, BRCMF_C_GET_VERSION, &io_type);
	if (err) {
		brcmf_err("Failed to get D11 version (%d)\n", err);
		goto priv_out;
	}
	cfg->d11inf.io_type = (u8)io_type;
	brcmu_d11_attach(&cfg->d11inf);

	err = brcmf_setup_wiphy(wiphy, ifp);
	if (err < 0)
		goto priv_out;

	brcmf_dbg(INFO, "Registering custom regulatory\n");
	wiphy->reg_notifier = brcmf_cfg80211_reg_notifier;
	wiphy->regulatory_flags |= REGULATORY_CUSTOM_REG;
	wiphy_apply_custom_regulatory(wiphy, &brcmf_regdom);

	/* firmware defaults to 40MHz disabled in 2G band. We signal
	 * cfg80211 here that we do and have it decide we can enable
	 * it. But first check if device does support 2G operation.
	 */
	if (wiphy->bands[NL80211_BAND_2GHZ]) {
		cap = &wiphy->bands[NL80211_BAND_2GHZ]->ht_cap.cap;
		*cap |= IEEE80211_HT_CAP_SUP_WIDTH_20_40;
	}
	err = wiphy_register(wiphy);
	if (err < 0) {
		brcmf_err("Could not register wiphy device (%d)\n", err);
		goto priv_out;
	}

	/* If cfg80211 didn't disable 40MHz HT CAP in wiphy_register(),
	 * setup 40MHz in 2GHz band and enable OBSS scanning.
	 */
	if (cap && (*cap & IEEE80211_HT_CAP_SUP_WIDTH_20_40)) {
		err = brcmf_enable_bw40_2g(cfg);
		if (!err)
			err = brcmf_fil_iovar_int_set(ifp, "obss_coex",
						      BRCMF_OBSS_COEX_AUTO);
		else
			*cap &= ~IEEE80211_HT_CAP_SUP_WIDTH_20_40;
	}
	/* p2p might require that "if-events" get processed by fweh. So
	 * activate the already registered event handlers now and activate
	 * the rest when initialization has completed. drvr->config needs to
	 * be assigned before activating events.
	 */
	drvr->config = cfg;
	err = brcmf_fweh_activate_events(ifp);
	if (err) {
		brcmf_err("FWEH activation failed (%d)\n", err);
		goto wiphy_unreg_out;
	}

	err = brcmf_p2p_attach(cfg, p2pdev_forced);
	if (err) {
		brcmf_err("P2P initilisation failed (%d)\n", err);
		goto wiphy_unreg_out;
	}
	err = brcmf_btcoex_attach(cfg);
	if (err) {
		brcmf_err("BT-coex initialisation failed (%d)\n", err);
		brcmf_p2p_detach(&cfg->p2p);
		goto wiphy_unreg_out;
	}

	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_TDLS)) {
		err = brcmf_fil_iovar_int_set(ifp, "tdls_enable", 1);
		if (err) {
			brcmf_dbg(INFO, "TDLS not enabled (%d)\n", err);
			wiphy->flags &= ~WIPHY_FLAG_SUPPORTS_TDLS;
		} else {
			brcmf_fweh_register(cfg->pub, BRCMF_E_TDLS_PEER_EVENT,
					    brcmf_notify_tdls_peer_event);
		}
	}

	/* (re-) activate FWEH event handling */
	err = brcmf_fweh_activate_events(ifp);
	if (err) {
		brcmf_err("FWEH activation failed (%d)\n", err);
		goto wiphy_unreg_out;
	}

	/* Fill in some of the advertised nl80211 supported features */
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_SCAN_RANDOM_MAC)) {
		wiphy->features |= NL80211_FEATURE_SCHED_SCAN_RANDOM_MAC_ADDR;
#ifdef CONFIG_PM
		if (wiphy->wowlan &&
		    wiphy->wowlan->flags & WIPHY_WOWLAN_NET_DETECT)
			wiphy->features |= NL80211_FEATURE_ND_RANDOM_MAC_ADDR;
#endif
	}

	return cfg;

wiphy_unreg_out:
	wiphy_unregister(cfg->wiphy);
priv_out:
	wl_deinit_priv(cfg);
	brcmf_free_vif(vif);
	ifp->vif = NULL;
wiphy_out:
	brcmf_free_wiphy(wiphy);
	kfree(ops);
	return NULL;
}
