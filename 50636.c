static void brcmf_wiphy_wowl_params(struct wiphy *wiphy, struct brcmf_if *ifp)
{
#ifdef CONFIG_PM
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);

	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_PNO)) {
		if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_ND)) {
			brcmf_wowlan_support.flags |= WIPHY_WOWLAN_NET_DETECT;
			init_waitqueue_head(&cfg->wowl.nd_data_wait);
		}
	}
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_WOWL_GTK)) {
		brcmf_wowlan_support.flags |= WIPHY_WOWLAN_SUPPORTS_GTK_REKEY;
		brcmf_wowlan_support.flags |= WIPHY_WOWLAN_GTK_REKEY_FAILURE;
	}

	wiphy->wowlan = &brcmf_wowlan_support;
#endif
}
