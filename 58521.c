struct wireless_dev *brcmf_ap_add_vif(struct wiphy *wiphy, const char *name,
				      struct vif_params *params)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_if *ifp = netdev_priv(cfg_to_ndev(cfg));
	struct brcmf_cfg80211_vif *vif;
	int err;

	if (brcmf_cfg80211_vif_event_armed(cfg))
		return ERR_PTR(-EBUSY);

	brcmf_dbg(INFO, "Adding vif \"%s\"\n", name);

	vif = brcmf_alloc_vif(cfg, NL80211_IFTYPE_AP);
	if (IS_ERR(vif))
		return (struct wireless_dev *)vif;

	brcmf_cfg80211_arm_vif_event(cfg, vif);

	err = brcmf_cfg80211_request_ap_if(ifp);
	if (err) {
		brcmf_cfg80211_arm_vif_event(cfg, NULL);
		goto fail;
	}

	/* wait for firmware event */
	err = brcmf_cfg80211_wait_vif_event(cfg, BRCMF_E_IF_ADD,
					    BRCMF_VIF_EVENT_TIMEOUT);
	brcmf_cfg80211_arm_vif_event(cfg, NULL);
	if (!err) {
		brcmf_err("timeout occurred\n");
		err = -EIO;
		goto fail;
	}

	/* interface created in firmware */
	ifp = vif->ifp;
	if (!ifp) {
		brcmf_err("no if pointer provided\n");
		err = -ENOENT;
		goto fail;
	}

	strncpy(ifp->ndev->name, name, sizeof(ifp->ndev->name) - 1);
	err = brcmf_net_attach(ifp, true);
	if (err) {
		brcmf_err("Registering netdevice failed\n");
		free_netdev(ifp->ndev);
		goto fail;
	}

	return &ifp->vif->wdev;

fail:
	brcmf_free_vif(vif);
	return ERR_PTR(err);
}
