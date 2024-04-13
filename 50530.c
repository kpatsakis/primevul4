static void brcmf_cfg80211_escan_timeout_worker(struct work_struct *work)
{
	struct brcmf_cfg80211_info *cfg =
			container_of(work, struct brcmf_cfg80211_info,
				     escan_timeout_work);

	brcmf_inform_bss(cfg);
	brcmf_notify_escan_complete(cfg, cfg->escan_info.ifp, true, true);
}
