struct ieee80211_sta *ieee80211_find_sta_by_ifaddr(struct ieee80211_hw *hw,
					       const u8 *addr,
					       const u8 *localaddr)
{
	struct sta_info *sta, *nxt;

	/*
	 * Just return a random station if localaddr is NULL
	 * ... first in list.
	 */
	for_each_sta_info(hw_to_local(hw), addr, sta, nxt) {
		if (localaddr &&
		    !ether_addr_equal(sta->sdata->vif.addr, localaddr))
			continue;
		if (!sta->uploaded)
			return NULL;
		return &sta->sta;
	}

	return NULL;
}
