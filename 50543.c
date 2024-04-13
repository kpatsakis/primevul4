static int brcmf_cfg80211_request_ap_if(struct brcmf_if *ifp)
{
	struct brcmf_mbss_ssid_le mbss_ssid_le;
	int bsscfgidx;
	int err;

	memset(&mbss_ssid_le, 0, sizeof(mbss_ssid_le));
	bsscfgidx = brcmf_get_first_free_bsscfgidx(ifp->drvr);
	if (bsscfgidx < 0)
		return bsscfgidx;

	mbss_ssid_le.bsscfgidx = cpu_to_le32(bsscfgidx);
	mbss_ssid_le.SSID_len = cpu_to_le32(5);
	sprintf(mbss_ssid_le.SSID, "ssid%d" , bsscfgidx);

	err = brcmf_fil_bsscfg_data_set(ifp, "bsscfg:ssid", &mbss_ssid_le,
					sizeof(mbss_ssid_le));
	if (err < 0)
		brcmf_err("setting ssid failed %d\n", err);

	return err;
}
