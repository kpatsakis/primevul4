static int brcmf_cfg80211_del_ap_iface(struct wiphy *wiphy,
				       struct wireless_dev *wdev)
{
	struct brcmf_cfg80211_info *cfg = wiphy_priv(wiphy);
	struct net_device *ndev = wdev->netdev;
	struct brcmf_if *ifp = netdev_priv(ndev);
	int ret;
	int err;

	brcmf_cfg80211_arm_vif_event(cfg, ifp->vif);

	err = brcmf_fil_bsscfg_data_set(ifp, "interface_remove", NULL, 0);
	if (err) {
		brcmf_err("interface_remove failed %d\n", err);
		goto err_unarm;
	}

	/* wait for firmware event */
	ret = brcmf_cfg80211_wait_vif_event(cfg, BRCMF_E_IF_DEL,
					    BRCMF_VIF_EVENT_TIMEOUT);
	if (!ret) {
		brcmf_err("timeout occurred\n");
		err = -EIO;
		goto err_unarm;
	}

	brcmf_remove_interface(ifp, true);

err_unarm:
	brcmf_cfg80211_arm_vif_event(cfg, NULL);
	return err;
}
