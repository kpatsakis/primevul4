s32 brcmf_notify_escan_complete(struct brcmf_cfg80211_info *cfg,
				struct brcmf_if *ifp, bool aborted,
				bool fw_abort)
{
	struct brcmf_scan_params_le params_le;
	struct cfg80211_scan_request *scan_request;
	s32 err = 0;

	brcmf_dbg(SCAN, "Enter\n");

	/* clear scan request, because the FW abort can cause a second call */
	/* to this functon and might cause a double cfg80211_scan_done      */
	scan_request = cfg->scan_request;
	cfg->scan_request = NULL;

	if (timer_pending(&cfg->escan_timeout))
		del_timer_sync(&cfg->escan_timeout);

	if (fw_abort) {
		/* Do a scan abort to stop the driver's scan engine */
		brcmf_dbg(SCAN, "ABORT scan in firmware\n");
		memset(&params_le, 0, sizeof(params_le));
		eth_broadcast_addr(params_le.bssid);
		params_le.bss_type = DOT11_BSSTYPE_ANY;
		params_le.scan_type = 0;
		params_le.channel_num = cpu_to_le32(1);
		params_le.nprobes = cpu_to_le32(1);
		params_le.active_time = cpu_to_le32(-1);
		params_le.passive_time = cpu_to_le32(-1);
		params_le.home_time = cpu_to_le32(-1);
		/* Scan is aborted by setting channel_list[0] to -1 */
		params_le.channel_list[0] = cpu_to_le16(-1);
		/* E-Scan (or anyother type) can be aborted by SCAN */
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SCAN,
					     &params_le, sizeof(params_le));
		if (err)
			brcmf_err("Scan abort  failed\n");
	}

	brcmf_scan_config_mpc(ifp, 1);

	/*
	 * e-scan can be initiated by scheduled scan
	 * which takes precedence.
	 */
	if (cfg->sched_escan) {
		brcmf_dbg(SCAN, "scheduled scan completed\n");
		cfg->sched_escan = false;
		if (!aborted)
			cfg80211_sched_scan_results(cfg_to_wiphy(cfg));
	} else if (scan_request) {
		struct cfg80211_scan_info info = {
			.aborted = aborted,
		};

		brcmf_dbg(SCAN, "ESCAN Completed scan: %s\n",
			  aborted ? "Aborted" : "Done");
		cfg80211_scan_done(scan_request, &info);
	}
	if (!test_and_clear_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status))
		brcmf_dbg(SCAN, "Scan complete, probably P2P scan\n");

	return err;
}
