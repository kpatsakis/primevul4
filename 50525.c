brcmf_cfg80211_del_station(struct wiphy *wiphy, struct net_device *ndev,
			   struct station_del_parameters *params)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_scb_val_le scbval;
	struct brcmf_if *ifp = netdev_priv(ndev);
	s32 err;

	if (!params->mac)
		return -EFAULT;

	brcmf_dbg(TRACE, "Enter %pM\n", params->mac);

	if (ifp->vif == cfg->p2p.bss_idx[P2PAPI_BSSCFG_DEVICE].vif)
		ifp = cfg->p2p.bss_idx[P2PAPI_BSSCFG_PRIMARY].vif->ifp;
	if (!check_vif_up(ifp->vif))
		return -EIO;

	memcpy(&scbval.ea, params->mac, ETH_ALEN);
	scbval.val = cpu_to_le32(params->reason_code);
	err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SCB_DEAUTHENTICATE_FOR_REASON,
				     &scbval, sizeof(scbval));
	if (err)
		brcmf_err("SCB_DEAUTHENTICATE_FOR_REASON failed %d\n", err);

	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
