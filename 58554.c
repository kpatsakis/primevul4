brcmf_notify_sched_scan_results(struct brcmf_if *ifp,
				const struct brcmf_event_msg *e, void *data)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	struct brcmf_pno_net_info_le *netinfo, *netinfo_start;
	struct cfg80211_scan_request *request = NULL;
	struct wiphy *wiphy = cfg_to_wiphy(cfg);
	int i, err = 0;
	struct brcmf_pno_scanresults_le *pfn_result;
	u32 bucket_map;
	u32 result_count;
	u32 status;
	u32 datalen;

	brcmf_dbg(SCAN, "Enter\n");

	if (e->datalen < (sizeof(*pfn_result) + sizeof(*netinfo))) {
		brcmf_dbg(SCAN, "Event data to small. Ignore\n");
		return 0;
	}

	if (e->event_code == BRCMF_E_PFN_NET_LOST) {
		brcmf_dbg(SCAN, "PFN NET LOST event. Do Nothing\n");
		return 0;
	}

	pfn_result = (struct brcmf_pno_scanresults_le *)data;
	result_count = le32_to_cpu(pfn_result->count);
	status = le32_to_cpu(pfn_result->status);

	/* PFN event is limited to fit 512 bytes so we may get
	 * multiple NET_FOUND events. For now place a warning here.
	 */
	WARN_ON(status != BRCMF_PNO_SCAN_COMPLETE);
	brcmf_dbg(SCAN, "PFN NET FOUND event. count: %d\n", result_count);
	if (!result_count) {
		brcmf_err("FALSE PNO Event. (pfn_count == 0)\n");
		goto out_err;
	}

	netinfo_start = brcmf_get_netinfo_array(pfn_result);
	datalen = e->datalen - ((void *)netinfo_start - (void *)pfn_result);
	if (datalen < result_count * sizeof(*netinfo)) {
		brcmf_err("insufficient event data\n");
		goto out_err;
	}

	request = brcmf_alloc_internal_escan_request(wiphy,
						     result_count);
	if (!request) {
		err = -ENOMEM;
		goto out_err;
	}

	bucket_map = 0;
	for (i = 0; i < result_count; i++) {
		netinfo = &netinfo_start[i];

		if (netinfo->SSID_len > IEEE80211_MAX_SSID_LEN)
			netinfo->SSID_len = IEEE80211_MAX_SSID_LEN;
		brcmf_dbg(SCAN, "SSID:%.32s Channel:%d\n",
			  netinfo->SSID, netinfo->channel);
		bucket_map |= brcmf_pno_get_bucket_map(cfg->pno, netinfo);
		err = brcmf_internal_escan_add_info(request,
						    netinfo->SSID,
						    netinfo->SSID_len,
						    netinfo->channel);
		if (err)
			goto out_err;
	}

	if (!bucket_map)
		goto free_req;

	err = brcmf_start_internal_escan(ifp, bucket_map, request);
	if (!err)
		goto free_req;

out_err:
	cfg80211_sched_scan_stopped(wiphy, 0);
free_req:
	kfree(request);
	return err;
}
