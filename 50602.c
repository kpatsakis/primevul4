brcmf_notify_roaming_status(struct brcmf_if *ifp,
			    const struct brcmf_event_msg *e, void *data)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	u32 event = e->event_code;
	u32 status = e->status;

	if (event == BRCMF_E_ROAM && status == BRCMF_E_STATUS_SUCCESS) {
		if (test_bit(BRCMF_VIF_STATUS_CONNECTED, &ifp->vif->sme_state))
			brcmf_bss_roaming_done(cfg, ifp->ndev, e);
		else
			brcmf_bss_connect_done(cfg, ifp->ndev, e, true);
	}

	return 0;
}
