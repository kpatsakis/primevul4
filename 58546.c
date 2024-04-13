static void brcmf_fill_bss_param(struct brcmf_if *ifp, struct station_info *si)
{
	struct {
		__le32 len;
		struct brcmf_bss_info_le bss_le;
	} *buf;
	u16 capability;
	int err;

	buf = kzalloc(WL_BSS_INFO_MAX, GFP_KERNEL);
	if (!buf)
		return;

	buf->len = cpu_to_le32(WL_BSS_INFO_MAX);
	err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_BSS_INFO, buf,
				     WL_BSS_INFO_MAX);
	if (err) {
		brcmf_err("Failed to get bss info (%d)\n", err);
		goto out_kfree;
	}
	si->filled |= BIT(NL80211_STA_INFO_BSS_PARAM);
	si->bss_param.beacon_interval = le16_to_cpu(buf->bss_le.beacon_period);
	si->bss_param.dtim_period = buf->bss_le.dtim_period;
	capability = le16_to_cpu(buf->bss_le.capability);
	if (capability & IEEE80211_HT_STBC_PARAM_DUAL_CTS_PROT)
		si->bss_param.flags |= BSS_PARAM_FLAGS_CTS_PROT;
	if (capability & WLAN_CAPABILITY_SHORT_PREAMBLE)
		si->bss_param.flags |= BSS_PARAM_FLAGS_SHORT_PREAMBLE;
	if (capability & WLAN_CAPABILITY_SHORT_SLOT_TIME)
		si->bss_param.flags |= BSS_PARAM_FLAGS_SHORT_SLOT_TIME;

out_kfree:
	kfree(buf);
}
