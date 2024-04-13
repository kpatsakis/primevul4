brcmf_notify_sched_scan_results(struct brcmf_if *ifp,
				const struct brcmf_event_msg *e, void *data)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	struct brcmf_pno_net_info_le *netinfo, *netinfo_start;
	struct cfg80211_scan_request *request = NULL;
	struct cfg80211_ssid *ssid = NULL;
	struct ieee80211_channel *channel = NULL;
	struct wiphy *wiphy = cfg_to_wiphy(cfg);
	int err = 0;
	int channel_req = 0;
	int band = 0;
	struct brcmf_pno_scanresults_le *pfn_result;
	u32 result_count;
	u32 status;

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
	if (result_count > 0) {
		int i;

		request = kzalloc(sizeof(*request), GFP_KERNEL);
		ssid = kcalloc(result_count, sizeof(*ssid), GFP_KERNEL);
		channel = kcalloc(result_count, sizeof(*channel), GFP_KERNEL);
		if (!request || !ssid || !channel) {
			err = -ENOMEM;
			goto out_err;
		}

		request->wiphy = wiphy;
		data += sizeof(struct brcmf_pno_scanresults_le);
		netinfo_start = (struct brcmf_pno_net_info_le *)data;

		for (i = 0; i < result_count; i++) {
			netinfo = &netinfo_start[i];
			if (!netinfo) {
				brcmf_err("Invalid netinfo ptr. index: %d\n",
					  i);
				err = -EINVAL;
				goto out_err;
			}

			brcmf_dbg(SCAN, "SSID:%s Channel:%d\n",
				  netinfo->SSID, netinfo->channel);
			memcpy(ssid[i].ssid, netinfo->SSID, netinfo->SSID_len);
			ssid[i].ssid_len = netinfo->SSID_len;
			request->n_ssids++;

			channel_req = netinfo->channel;
			if (channel_req <= CH_MAX_2G_CHANNEL)
				band = NL80211_BAND_2GHZ;
			else
				band = NL80211_BAND_5GHZ;
			channel[i].center_freq =
				ieee80211_channel_to_frequency(channel_req,
							       band);
			channel[i].band = band;
			channel[i].flags |= IEEE80211_CHAN_NO_HT40;
			request->channels[i] = &channel[i];
			request->n_channels++;
		}

		/* assign parsed ssid array */
		if (request->n_ssids)
			request->ssids = &ssid[0];

		if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
			/* Abort any on-going scan */
			brcmf_abort_scanning(cfg);
		}

		set_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
		cfg->escan_info.run = brcmf_run_escan;
		err = brcmf_do_escan(cfg, wiphy, ifp, request);
		if (err) {
			clear_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
			goto out_err;
		}
		cfg->sched_escan = true;
		cfg->scan_request = request;
	} else {
		brcmf_err("FALSE PNO Event. (pfn_count == 0)\n");
		goto out_err;
	}

	kfree(ssid);
	kfree(channel);
	kfree(request);
	return 0;

out_err:
	kfree(ssid);
	kfree(channel);
	kfree(request);
	cfg80211_sched_scan_stopped(wiphy);
	return err;
}
