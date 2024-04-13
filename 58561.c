static int brcmf_vif_add_validate(struct brcmf_cfg80211_info *cfg,
				  enum nl80211_iftype new_type)
{
	struct brcmf_cfg80211_vif *pos;
	struct iface_combination_params params = {
		.num_different_channels = 1,
	};

	list_for_each_entry(pos, &cfg->vif_list, list)
		params.iftype_num[pos->wdev.iftype]++;

	params.iftype_num[new_type]++;
	return cfg80211_check_combinations(cfg->wiphy, &params);
}
