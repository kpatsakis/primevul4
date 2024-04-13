bool brcmf_get_vif_state_any(struct brcmf_cfg80211_info *cfg,
			     unsigned long state)
{
	struct brcmf_cfg80211_vif *vif;

	list_for_each_entry(vif, &cfg->vif_list, list) {
		if (test_bit(state, &vif->sme_state))
			return true;
	}
	return false;
}
