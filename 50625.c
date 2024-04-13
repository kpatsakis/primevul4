static s32 brcmf_update_bss_info(struct brcmf_cfg80211_info *cfg,
				 struct brcmf_if *ifp)
{
	struct brcmf_bss_info_le *bi;
	const struct brcmf_tlv *tim;
	u16 beacon_interval;
	u8 dtim_period;
	size_t ie_len;
	u8 *ie;
	s32 err = 0;

	brcmf_dbg(TRACE, "Enter\n");
	if (brcmf_is_ibssmode(ifp->vif))
		return err;

	*(__le32 *)cfg->extra_buf = cpu_to_le32(WL_EXTRA_BUF_MAX);
	err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_BSS_INFO,
				     cfg->extra_buf, WL_EXTRA_BUF_MAX);
	if (err) {
		brcmf_err("Could not get bss info %d\n", err);
		goto update_bss_info_out;
	}

	bi = (struct brcmf_bss_info_le *)(cfg->extra_buf + 4);
	err = brcmf_inform_single_bss(cfg, bi);
	if (err)
		goto update_bss_info_out;

	ie = ((u8 *)bi) + le16_to_cpu(bi->ie_offset);
	ie_len = le32_to_cpu(bi->ie_length);
	beacon_interval = le16_to_cpu(bi->beacon_period);

	tim = brcmf_parse_tlvs(ie, ie_len, WLAN_EID_TIM);
	if (tim)
		dtim_period = tim->data[1];
	else {
		/*
		* active scan was done so we could not get dtim
		* information out of probe response.
		* so we speficially query dtim information to dongle.
		*/
		u32 var;
		err = brcmf_fil_iovar_int_get(ifp, "dtim_assoc", &var);
		if (err) {
			brcmf_err("wl dtim_assoc failed (%d)\n", err);
			goto update_bss_info_out;
		}
		dtim_period = (u8)var;
	}

update_bss_info_out:
	brcmf_dbg(TRACE, "Exit");
	return err;
}
