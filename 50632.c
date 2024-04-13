static int brcmf_vif_change_validate(struct brcmf_cfg80211_info *cfg,
				     struct brcmf_cfg80211_vif *vif,
				     enum nl80211_iftype new_type)
{
	int iftype_num[NUM_NL80211_IFTYPES];
	struct brcmf_cfg80211_vif *pos;
	bool check_combos = false;
	int ret = 0;

	memset(&iftype_num[0], 0, sizeof(iftype_num));
	list_for_each_entry(pos, &cfg->vif_list, list)
		if (pos == vif) {
			iftype_num[new_type]++;
		} else {
			/* concurrent interfaces so need check combinations */
			check_combos = true;
			iftype_num[pos->wdev.iftype]++;
		}

	if (check_combos)
		ret = cfg80211_check_combinations(cfg->wiphy, 1, 0, iftype_num);

	return ret;
}
