static void brcmf_escan_prep(struct brcmf_cfg80211_info *cfg,
			     struct brcmf_scan_params_le *params_le,
			     struct cfg80211_scan_request *request)
{
	u32 n_ssids;
	u32 n_channels;
	s32 i;
	s32 offset;
	u16 chanspec;
	char *ptr;
	struct brcmf_ssid_le ssid_le;

	eth_broadcast_addr(params_le->bssid);
	params_le->bss_type = DOT11_BSSTYPE_ANY;
	params_le->scan_type = 0;
	params_le->channel_num = 0;
	params_le->nprobes = cpu_to_le32(-1);
	params_le->active_time = cpu_to_le32(-1);
	params_le->passive_time = cpu_to_le32(-1);
	params_le->home_time = cpu_to_le32(-1);
	memset(&params_le->ssid_le, 0, sizeof(params_le->ssid_le));

	/* if request is null exit so it will be all channel broadcast scan */
	if (!request)
		return;

	n_ssids = request->n_ssids;
	n_channels = request->n_channels;
	/* Copy channel array if applicable */
	brcmf_dbg(SCAN, "### List of channelspecs to scan ### %d\n",
		  n_channels);
	if (n_channels > 0) {
		for (i = 0; i < n_channels; i++) {
			chanspec = channel_to_chanspec(&cfg->d11inf,
						       request->channels[i]);
			brcmf_dbg(SCAN, "Chan : %d, Channel spec: %x\n",
				  request->channels[i]->hw_value, chanspec);
			params_le->channel_list[i] = cpu_to_le16(chanspec);
		}
	} else {
		brcmf_dbg(SCAN, "Scanning all channels\n");
	}
	/* Copy ssid array if applicable */
	brcmf_dbg(SCAN, "### List of SSIDs to scan ### %d\n", n_ssids);
	if (n_ssids > 0) {
		offset = offsetof(struct brcmf_scan_params_le, channel_list) +
				n_channels * sizeof(u16);
		offset = roundup(offset, sizeof(u32));
		ptr = (char *)params_le + offset;
		for (i = 0; i < n_ssids; i++) {
			memset(&ssid_le, 0, sizeof(ssid_le));
			ssid_le.SSID_len =
					cpu_to_le32(request->ssids[i].ssid_len);
			memcpy(ssid_le.SSID, request->ssids[i].ssid,
			       request->ssids[i].ssid_len);
			if (!ssid_le.SSID_len)
				brcmf_dbg(SCAN, "%d: Broadcast scan\n", i);
			else
				brcmf_dbg(SCAN, "%d: scan for  %s size =%d\n",
					  i, ssid_le.SSID, ssid_le.SSID_len);
			memcpy(ptr, &ssid_le, sizeof(ssid_le));
			ptr += sizeof(ssid_le);
		}
	} else {
		brcmf_dbg(SCAN, "Broadcast scan %p\n", request->ssids);
		if ((request->ssids) && request->ssids->ssid_len) {
			brcmf_dbg(SCAN, "SSID %s len=%d\n",
				  params_le->ssid_le.SSID,
				  request->ssids->ssid_len);
			params_le->ssid_le.SSID_len =
				cpu_to_le32(request->ssids->ssid_len);
			memcpy(&params_le->ssid_le.SSID, request->ssids->ssid,
				request->ssids->ssid_len);
		}
	}
	/* Adding mask to channel numbers */
	params_le->channel_num =
		cpu_to_le32((n_ssids << BRCMF_SCAN_PARAMS_NSSID_SHIFT) |
			(n_channels & BRCMF_SCAN_PARAMS_COUNT_MASK));
}
