brcmf_run_escan(struct brcmf_cfg80211_info *cfg, struct brcmf_if *ifp,
		struct cfg80211_scan_request *request)
{
	s32 params_size = BRCMF_SCAN_PARAMS_FIXED_SIZE +
			  offsetof(struct brcmf_escan_params_le, params_le);
	struct brcmf_escan_params_le *params;
	s32 err = 0;

	brcmf_dbg(SCAN, "E-SCAN START\n");

	if (request != NULL) {
		/* Allocate space for populating ssids in struct */
		params_size += sizeof(u32) * ((request->n_channels + 1) / 2);

		/* Allocate space for populating ssids in struct */
		params_size += sizeof(struct brcmf_ssid_le) * request->n_ssids;
	}

	params = kzalloc(params_size, GFP_KERNEL);
	if (!params) {
		err = -ENOMEM;
		goto exit;
	}
	BUG_ON(params_size + sizeof("escan") >= BRCMF_DCMD_MEDLEN);
	brcmf_escan_prep(cfg, &params->params_le, request);
	params->version = cpu_to_le32(BRCMF_ESCAN_REQ_VERSION);
	params->action = cpu_to_le16(WL_ESCAN_ACTION_START);
	params->sync_id = cpu_to_le16(0x1234);

	err = brcmf_fil_iovar_data_set(ifp, "escan", params, params_size);
	if (err) {
		if (err == -EBUSY)
			brcmf_dbg(INFO, "system busy : escan canceled\n");
		else
			brcmf_err("error (%d)\n", err);
	}

	kfree(params);
exit:
	return err;
}
