static void brcmf_wiphy_wowl_params(struct wiphy *wiphy, struct brcmf_if *ifp)
{
#ifdef CONFIG_PM
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct wiphy_wowlan_support *wowl;

	wowl = kmemdup(&brcmf_wowlan_support, sizeof(brcmf_wowlan_support),
		       GFP_KERNEL);
	if (!wowl) {
		brcmf_err("only support basic wowlan features\n");
		wiphy->wowlan = &brcmf_wowlan_support;
		return;
	}

	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_PNO)) {
		if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_ND)) {
			wowl->flags |= WIPHY_WOWLAN_NET_DETECT;
			wowl->max_nd_match_sets = BRCMF_PNO_MAX_PFN_COUNT;
			init_waitqueue_head(&cfg->wowl.nd_data_wait);
		}
	}
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_GTK)) {
		wowl->flags |= WIPHY_WOWLAN_SUPPORTS_GTK_REKEY;
		wowl->flags |= WIPHY_WOWLAN_GTK_REKEY_FAILURE;
	}

	wiphy->wowlan = wowl;
#endif
}
