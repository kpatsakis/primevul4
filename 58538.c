static int brcmf_cfg80211_stop_ap(struct wiphy *wiphy, struct net_device *ndev)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	s32 err;
	struct brcmf_fil_bss_enable_le bss_enable;
	struct brcmf_join_params join_params;

	brcmf_dbg(TRACE, "Enter\n");

	if (ifp->vif->wdev.iftype == NL80211_IFTYPE_AP) {
		/* Due to most likely deauths outstanding we sleep */
		/* first to make sure they get processed by fw. */
		msleep(400);

		if (ifp->vif->mbss) {
			err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_DOWN, 1);
			return err;
		}

		/* First BSS doesn't get a full reset */
		if (ifp->bsscfgidx == 0)
			brcmf_fil_iovar_int_set(ifp, "closednet", 0);

		memset(&join_params, 0, sizeof(join_params));
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_SSID,
					     &join_params, sizeof(join_params));
		if (err < 0)
			brcmf_err("SET SSID error (%d)\n", err);
		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_DOWN, 1);
		if (err < 0)
			brcmf_err("BRCMF_C_DOWN error %d\n", err);
		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_AP, 0);
		if (err < 0)
			brcmf_err("setting AP mode failed %d\n", err);
		if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MBSS))
			brcmf_fil_iovar_int_set(ifp, "mbss", 0);
		brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_REGULATORY,
				      ifp->vif->is_11d);
		/* Bring device back up so it can be used again */
		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_UP, 1);
		if (err < 0)
			brcmf_err("BRCMF_C_UP error %d\n", err);

		brcmf_vif_clear_mgmt_ies(ifp->vif);
	} else {
		bss_enable.bsscfgidx = cpu_to_le32(ifp->bsscfgidx);
		bss_enable.enable = cpu_to_le32(0);
		err = brcmf_fil_iovar_data_set(ifp, "bss", &bss_enable,
					       sizeof(bss_enable));
		if (err < 0)
			brcmf_err("bss_enable config failed %d\n", err);
	}
	brcmf_set_mpc(ifp, 1);
	brcmf_configure_arp_nd_offload(ifp, true);
	clear_bit(BRCMF_VIF_STATUS_AP_CREATED, &ifp->vif->sme_state);
	brcmf_net_setcarrier(ifp, false);

	return err;
}
