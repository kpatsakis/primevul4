static int ieee80211_beacon_add_tim(struct ieee80211_sub_if_data *sdata,
				    struct ps_data *ps, struct sk_buff *skb)
{
	struct ieee80211_local *local = sdata->local;

	/*
	 * Not very nice, but we want to allow the driver to call
	 * ieee80211_beacon_get() as a response to the set_tim()
	 * callback. That, however, is already invoked under the
	 * sta_lock to guarantee consistent and race-free update
	 * of the tim bitmap in mac80211 and the driver.
	 */
	if (local->tim_in_locked_section) {
		__ieee80211_beacon_add_tim(sdata, ps, skb);
	} else {
		spin_lock_bh(&local->tim_lock);
		__ieee80211_beacon_add_tim(sdata, ps, skb);
		spin_unlock_bh(&local->tim_lock);
	}

	return 0;
}
