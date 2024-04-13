static void brcmf_escan_timeout(unsigned long data)
{
	struct brcmf_cfg80211_info *cfg =
			(struct brcmf_cfg80211_info *)data;

	if (cfg->int_escan_map || cfg->scan_request) {
		brcmf_err("timer expired\n");
		schedule_work(&cfg->escan_timeout_work);
	}
}
