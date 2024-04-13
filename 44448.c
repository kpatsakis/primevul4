static void ath_set_rates(struct ieee80211_vif *vif, struct ieee80211_sta *sta,
			  struct ath_buf *bf)
{
	ieee80211_get_tx_rates(vif, sta, bf->bf_mpdu, bf->rates,
			       ARRAY_SIZE(bf->rates));
}
