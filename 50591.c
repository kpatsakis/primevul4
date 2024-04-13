static s32 brcmf_init_priv_mem(struct brcmf_cfg80211_info *cfg)
{
	cfg->conf = kzalloc(sizeof(*cfg->conf), GFP_KERNEL);
	if (!cfg->conf)
		goto init_priv_mem_out;
	cfg->extra_buf = kzalloc(WL_EXTRA_BUF_MAX, GFP_KERNEL);
	if (!cfg->extra_buf)
		goto init_priv_mem_out;
	cfg->wowl.nd = kzalloc(sizeof(*cfg->wowl.nd) + sizeof(u32), GFP_KERNEL);
	if (!cfg->wowl.nd)
		goto init_priv_mem_out;
	cfg->wowl.nd_info = kzalloc(sizeof(*cfg->wowl.nd_info) +
				    sizeof(struct cfg80211_wowlan_nd_match *),
				    GFP_KERNEL);
	if (!cfg->wowl.nd_info)
		goto init_priv_mem_out;
	cfg->escan_info.escan_buf = kzalloc(BRCMF_ESCAN_BUF_SIZE, GFP_KERNEL);
	if (!cfg->escan_info.escan_buf)
		goto init_priv_mem_out;

	return 0;

init_priv_mem_out:
	brcmf_deinit_priv_mem(cfg);

	return -ENOMEM;
}
