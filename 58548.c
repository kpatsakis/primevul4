static int brcmf_internal_escan_add_info(struct cfg80211_scan_request *req,
					 u8 *ssid, u8 ssid_len, u8 channel)
{
	struct ieee80211_channel *chan;
	enum nl80211_band band;
	int freq, i;

	if (channel <= CH_MAX_2G_CHANNEL)
		band = NL80211_BAND_2GHZ;
	else
		band = NL80211_BAND_5GHZ;

	freq = ieee80211_channel_to_frequency(channel, band);
	if (!freq)
		return -EINVAL;

	chan = ieee80211_get_channel(req->wiphy, freq);
	if (!chan)
		return -EINVAL;

	for (i = 0; i < req->n_channels; i++) {
		if (req->channels[i] == chan)
			break;
	}
	if (i == req->n_channels)
		req->channels[req->n_channels++] = chan;

	for (i = 0; i < req->n_ssids; i++) {
		if (req->ssids[i].ssid_len == ssid_len &&
		    !memcmp(req->ssids[i].ssid, ssid, ssid_len))
			break;
	}
	if (i == req->n_ssids) {
		memcpy(req->ssids[req->n_ssids].ssid, ssid, ssid_len);
		req->ssids[req->n_ssids++].ssid_len = ssid_len;
	}
	return 0;
}
