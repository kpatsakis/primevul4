brcmf_cfg80211_update_proto_addr_mode(struct wireless_dev *wdev)
{
	struct brcmf_cfg80211_vif *vif;
	struct brcmf_if *ifp;

	vif = container_of(wdev, struct brcmf_cfg80211_vif, wdev);
	ifp = vif->ifp;

	if ((wdev->iftype == NL80211_IFTYPE_ADHOC) ||
	    (wdev->iftype == NL80211_IFTYPE_AP) ||
	    (wdev->iftype == NL80211_IFTYPE_P2P_GO))
		brcmf_proto_configure_addr_mode(ifp->drvr, ifp->ifidx,
						ADDR_DIRECT);
	else
		brcmf_proto_configure_addr_mode(ifp->drvr, ifp->ifidx,
						ADDR_INDIRECT);
}
