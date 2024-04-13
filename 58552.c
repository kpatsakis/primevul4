brcmf_notify_connect_status_ap(struct brcmf_cfg80211_info *cfg,
			       struct net_device *ndev,
			       const struct brcmf_event_msg *e, void *data)
{
	static int generation;
	u32 event = e->event_code;
	u32 reason = e->reason;
	struct station_info sinfo;

	brcmf_dbg(CONN, "event %s (%u), reason %d\n",
		  brcmf_fweh_event_name(event), event, reason);
	if (event == BRCMF_E_LINK && reason == BRCMF_E_REASON_LINK_BSSCFG_DIS &&
	    ndev != cfg_to_ndev(cfg)) {
		brcmf_dbg(CONN, "AP mode link down\n");
		complete(&cfg->vif_disabled);
		return 0;
	}

	if (((event == BRCMF_E_ASSOC_IND) || (event == BRCMF_E_REASSOC_IND)) &&
	    (reason == BRCMF_E_STATUS_SUCCESS)) {
		memset(&sinfo, 0, sizeof(sinfo));
		if (!data) {
			brcmf_err("No IEs present in ASSOC/REASSOC_IND");
			return -EINVAL;
		}
		sinfo.assoc_req_ies = data;
		sinfo.assoc_req_ies_len = e->datalen;
		generation++;
		sinfo.generation = generation;
		cfg80211_new_sta(ndev, e->addr, &sinfo, GFP_KERNEL);
	} else if ((event == BRCMF_E_DISASSOC_IND) ||
		   (event == BRCMF_E_DEAUTH_IND) ||
		   (event == BRCMF_E_DEAUTH)) {
		cfg80211_del_sta(ndev, e->addr, GFP_KERNEL);
	}
	return 0;
}
