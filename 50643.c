static u8 nl80211_band_to_fwil(enum nl80211_band band)
{
	switch (band) {
	case NL80211_BAND_2GHZ:
		return WLC_BAND_2G;
	case NL80211_BAND_5GHZ:
		return WLC_BAND_5G;
	default:
		WARN_ON(1);
		break;
	}
	return 0;
}
