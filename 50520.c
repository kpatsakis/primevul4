static int brcmf_cfg80211_crit_proto_start(struct wiphy *wiphy,
					   struct wireless_dev *wdev,
					   enum nl80211_crit_proto_id proto,
					   u16 duration)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_cfg80211_vif *vif;

	vif = container_of(wdev, struct brcmf_cfg80211_vif, wdev);

	/* only DHCP support for now */
	if (proto != NL80211_CRIT_PROTO_DHCP)
		return -EINVAL;

	/* suppress and abort scanning */
	set_bit(BRCMF_SCAN_STATUS_SUPPRESS, &cfg->scan_status);
	brcmf_abort_scanning(cfg);

	return brcmf_btcoex_set_mode(vif, BRCMF_BTCOEX_DISABLED, duration);
}
