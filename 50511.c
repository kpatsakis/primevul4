static struct wireless_dev *brcmf_cfg80211_add_iface(struct wiphy *wiphy,
						     const char *name,
						     unsigned char name_assign_type,
						     enum nl80211_iftype type,
						     u32 *flags,
						     struct vif_params *params)
{
	struct wireless_dev *wdev;
	int err;

	brcmf_dbg(TRACE, "enter: %s type %d\n", name, type);
	err = brcmf_vif_add_validate(wiphy_to_cfg(wiphy), type);
	if (err) {
		brcmf_err("iface validation failed: err=%d\n", err);
		return ERR_PTR(err);
	}
	switch (type) {
	case NL80211_IFTYPE_ADHOC:
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_WDS:
	case NL80211_IFTYPE_MONITOR:
	case NL80211_IFTYPE_MESH_POINT:
		return ERR_PTR(-EOPNOTSUPP);
	case NL80211_IFTYPE_AP:
		wdev = brcmf_ap_add_vif(wiphy, name, flags, params);
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
	case NL80211_IFTYPE_P2P_GO:
	case NL80211_IFTYPE_P2P_DEVICE:
		wdev = brcmf_p2p_add_vif(wiphy, name, name_assign_type, type, flags, params);
		break;
	case NL80211_IFTYPE_UNSPECIFIED:
	default:
		return ERR_PTR(-EINVAL);
	}

	if (IS_ERR(wdev))
		brcmf_err("add iface %s type %d failed: err=%d\n",
			  name, type, (int)PTR_ERR(wdev));
	else
		brcmf_cfg80211_update_proto_addr_mode(wdev);

	return wdev;
}
