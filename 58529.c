void brcmf_cfg80211_detach(struct brcmf_cfg80211_info *cfg)
{
	if (!cfg)
		return;

	brcmf_pno_detach(cfg);
	brcmf_btcoex_detach(cfg);
	wiphy_unregister(cfg->wiphy);
	kfree(cfg->ops);
	wl_deinit_priv(cfg);
	brcmf_free_wiphy(cfg->wiphy);
}
