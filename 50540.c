brcmf_cfg80211_mgmt_frame_register(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   u16 frame_type, bool reg)
{
	struct brcmf_cfg80211_vif *vif;
	u16 mgmt_type;

	brcmf_dbg(TRACE, "Enter, frame_type %04x, reg=%d\n", frame_type, reg);

	mgmt_type = (frame_type & IEEE80211_FCTL_STYPE) >> 4;
	vif = container_of(wdev, struct brcmf_cfg80211_vif, wdev);
	if (reg)
		vif->mgmt_rx_reg |= BIT(mgmt_type);
	else
		vif->mgmt_rx_reg &= ~BIT(mgmt_type);
}
