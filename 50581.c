static void brcmf_free_wiphy(struct wiphy *wiphy)
{
	int i;

	if (!wiphy)
		return;

	if (wiphy->iface_combinations) {
		for (i = 0; i < wiphy->n_iface_combinations; i++)
			kfree(wiphy->iface_combinations[i].limits);
	}
	kfree(wiphy->iface_combinations);
	if (wiphy->bands[NL80211_BAND_2GHZ]) {
		kfree(wiphy->bands[NL80211_BAND_2GHZ]->channels);
		kfree(wiphy->bands[NL80211_BAND_2GHZ]);
	}
	if (wiphy->bands[NL80211_BAND_5GHZ]) {
		kfree(wiphy->bands[NL80211_BAND_5GHZ]->channels);
		kfree(wiphy->bands[NL80211_BAND_5GHZ]);
	}
	wiphy_free(wiphy);
}
