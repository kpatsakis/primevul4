brcmf_do_escan(struct brcmf_if *ifp, struct cfg80211_scan_request *request)
{
	struct brcmf_cfg80211_info *cfg = ifp->drvr->config;
	s32 err;
	u32 passive_scan;
	struct brcmf_scan_results *results;
	struct escan_info *escan = &cfg->escan_info;

	brcmf_dbg(SCAN, "Enter\n");
	escan->ifp = ifp;
	escan->wiphy = cfg->wiphy;
	escan->escan_state = WL_ESCAN_STATE_SCANNING;
	passive_scan = cfg->active_scan ? 0 : 1;
	err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_PASSIVE_SCAN,
				    passive_scan);
	if (err) {
		brcmf_err("error (%d)\n", err);
		return err;
	}
	brcmf_scan_config_mpc(ifp, 0);
	results = (struct brcmf_scan_results *)cfg->escan_info.escan_buf;
	results->version = 0;
	results->count = 0;
	results->buflen = WL_ESCAN_RESULTS_FIXED_SIZE;

	err = escan->run(cfg, ifp, request);
	if (err)
		brcmf_scan_config_mpc(ifp, 1);
	return err;
}
