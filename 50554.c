static int brcmf_cfg80211_tdls_oper(struct wiphy *wiphy,
				    struct net_device *ndev, const u8 *peer,
				    enum nl80211_tdls_operation oper)
{
	struct brcmf_if *ifp;
	struct brcmf_tdls_iovar_le info;
	int ret = 0;

	ret = brcmf_convert_nl80211_tdls_oper(oper);
	if (ret < 0)
		return ret;

	ifp = netdev_priv(ndev);
	memset(&info, 0, sizeof(info));
	info.mode = (u8)ret;
	if (peer)
		memcpy(info.ea, peer, ETH_ALEN);

	ret = brcmf_fil_iovar_data_set(ifp, "tdls_endpoint",
				       &info, sizeof(info));
	if (ret < 0)
		brcmf_err("tdls_endpoint iovar failed: ret=%d\n", ret);

	return ret;
}
