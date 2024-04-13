static void brcmf_deinit_priv_mem(struct brcmf_cfg80211_info *cfg)
{
	kfree(cfg->conf);
	cfg->conf = NULL;
	kfree(cfg->extra_buf);
	cfg->extra_buf = NULL;
	kfree(cfg->wowl.nd);
	cfg->wowl.nd = NULL;
	kfree(cfg->wowl.nd_info);
	cfg->wowl.nd_info = NULL;
	kfree(cfg->escan_info.escan_buf);
	cfg->escan_info.escan_buf = NULL;
}
