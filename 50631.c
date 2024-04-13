static int brcmf_vif_add_validate(struct brcmf_cfg80211_info *cfg,
				  enum nl80211_iftype new_type)
{
	int iftype_num[NUM_NL80211_IFTYPES];
	struct brcmf_cfg80211_vif *pos;

	memset(&iftype_num[0], 0, sizeof(iftype_num));
	list_for_each_entry(pos, &cfg->vif_list, list)
		iftype_num[pos->wdev.iftype]++;

	iftype_num[new_type]++;
	return cfg80211_check_combinations(cfg->wiphy, 1, 0, iftype_num);
}
