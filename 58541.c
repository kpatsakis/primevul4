static void brcmf_configure_wowl(struct brcmf_cfg80211_info *cfg,
				 struct brcmf_if *ifp,
				 struct cfg80211_wowlan *wowl)
{
	u32 wowl_config;
	struct brcmf_wowl_wakeind_le wowl_wakeind;
	u32 i;

	brcmf_dbg(TRACE, "Suspend, wowl config.\n");

	if (!brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_ARP_ND))
		brcmf_configure_arp_nd_offload(ifp, false);
	brcmf_fil_cmd_int_get(ifp, BRCMF_C_GET_PM, &cfg->wowl.pre_pmmode);
	brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_PM, PM_MAX);

	wowl_config = 0;
	if (wowl->disconnect)
		wowl_config = BRCMF_WOWL_DIS | BRCMF_WOWL_BCN | BRCMF_WOWL_RETR;
	if (wowl->magic_pkt)
		wowl_config |= BRCMF_WOWL_MAGIC;
	if ((wowl->patterns) && (wowl->n_patterns)) {
		wowl_config |= BRCMF_WOWL_NET;
		for (i = 0; i < wowl->n_patterns; i++) {
			brcmf_config_wowl_pattern(ifp, "add",
				(u8 *)wowl->patterns[i].pattern,
				wowl->patterns[i].pattern_len,
				(u8 *)wowl->patterns[i].mask,
				wowl->patterns[i].pkt_offset);
		}
	}
	if (wowl->nd_config) {
		brcmf_cfg80211_sched_scan_start(cfg->wiphy, ifp->ndev,
						wowl->nd_config);
		wowl_config |= BRCMF_WOWL_PFN_FOUND;

		cfg->wowl.nd_data_completed = false;
		cfg->wowl.nd_enabled = true;
		/* Now reroute the event for PFN to the wowl function. */
		brcmf_fweh_unregister(cfg->pub, BRCMF_E_PFN_NET_FOUND);
		brcmf_fweh_register(cfg->pub, BRCMF_E_PFN_NET_FOUND,
				    brcmf_wowl_nd_results);
	}
	if (wowl->gtk_rekey_failure)
		wowl_config |= BRCMF_WOWL_GTK_FAILURE;
	if (!test_bit(BRCMF_VIF_STATUS_CONNECTED, &ifp->vif->sme_state))
		wowl_config |= BRCMF_WOWL_UNASSOC;

	memcpy(&wowl_wakeind, "clear", 6);
	brcmf_fil_iovar_data_set(ifp, "wowl_wakeind", &wowl_wakeind,
				 sizeof(wowl_wakeind));
	brcmf_fil_iovar_int_set(ifp, "wowl", wowl_config);
	brcmf_fil_iovar_int_set(ifp, "wowl_activate", 1);
	brcmf_bus_wowl_config(cfg->pub->bus_if, true);
	cfg->wowl.active = true;
}
