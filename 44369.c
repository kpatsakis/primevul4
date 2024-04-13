void ieee80211_sta_ps_deliver_poll_response(struct sta_info *sta)
{
	u8 ignore_for_response = sta->sta.uapsd_queues;

	/*
	 * If all ACs are delivery-enabled then we should reply
	 * from any of them, if only some are enabled we reply
	 * only from the non-enabled ones.
	 */
	if (ignore_for_response == BIT(IEEE80211_NUM_ACS) - 1)
		ignore_for_response = 0;

	ieee80211_sta_ps_deliver_response(sta, 1, ignore_for_response,
					  IEEE80211_FRAME_RELEASE_PSPOLL);
}
