static int brcmf_cfg80211_get_channel(struct wiphy *wiphy,
				      struct wireless_dev *wdev,
				      struct cfg80211_chan_def *chandef)
{
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct net_device *ndev = wdev->netdev;
	struct brcmf_if *ifp;
	struct brcmu_chan ch;
	enum nl80211_band band = 0;
	enum nl80211_chan_width width = 0;
	u32 chanspec;
	int freq, err;

	if (!ndev)
		return -ENODEV;
	ifp = netdev_priv(ndev);

	err = brcmf_fil_iovar_int_get(ifp, "chanspec", &chanspec);
	if (err) {
		brcmf_err("chanspec failed (%d)\n", err);
		return err;
	}

	ch.chspec = chanspec;
	cfg->d11inf.decchspec(&ch);

	switch (ch.band) {
	case BRCMU_CHAN_BAND_2G:
		band = NL80211_BAND_2GHZ;
		break;
	case BRCMU_CHAN_BAND_5G:
		band = NL80211_BAND_5GHZ;
		break;
	}

	switch (ch.bw) {
	case BRCMU_CHAN_BW_80:
		width = NL80211_CHAN_WIDTH_80;
		break;
	case BRCMU_CHAN_BW_40:
		width = NL80211_CHAN_WIDTH_40;
		break;
	case BRCMU_CHAN_BW_20:
		width = NL80211_CHAN_WIDTH_20;
		break;
	case BRCMU_CHAN_BW_80P80:
		width = NL80211_CHAN_WIDTH_80P80;
		break;
	case BRCMU_CHAN_BW_160:
		width = NL80211_CHAN_WIDTH_160;
		break;
	}

	freq = ieee80211_channel_to_frequency(ch.control_ch_num, band);
	chandef->chan = ieee80211_get_channel(wiphy, freq);
	chandef->width = width;
	chandef->center_freq1 = ieee80211_channel_to_frequency(ch.chnum, band);
	chandef->center_freq2 = 0;

	return 0;
}
