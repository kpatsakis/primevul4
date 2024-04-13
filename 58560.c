static int brcmf_start_internal_escan(struct brcmf_if *ifp, u32 fwmap,
				      struct cfg80211_scan_request *request)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	int err;

	if (test_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status)) {
		if (cfg->int_escan_map)
			brcmf_dbg(SCAN, "aborting internal scan: map=%u\n",
				  cfg->int_escan_map);
		/* Abort any on-going scan */
		brcmf_abort_scanning(cfg);
	}

	brcmf_dbg(SCAN, "start internal scan: map=%u\n", fwmap);
	set_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
	cfg->escan_info.run = brcmf_run_escan;
	err = brcmf_do_escan(ifp, request);
	if (err) {
		clear_bit(BRCMF_SCAN_STATUS_BUSY, &cfg->scan_status);
		return err;
	}
	cfg->int_escan_map = fwmap;
	return 0;
}
