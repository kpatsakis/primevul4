struct sta_info *sta_info_get_bss(struct ieee80211_sub_if_data *sdata,
				  const u8 *addr)
{
	struct ieee80211_local *local = sdata->local;
	struct sta_info *sta;

	sta = rcu_dereference_check(local->sta_hash[STA_HASH(addr)],
				    lockdep_is_held(&local->sta_mtx));
	while (sta) {
		if ((sta->sdata == sdata ||
		     (sta->sdata->bss && sta->sdata->bss == sdata->bss)) &&
		    ether_addr_equal(sta->sta.addr, addr))
			break;
		sta = rcu_dereference_check(sta->hnext,
					    lockdep_is_held(&local->sta_mtx));
	}
	return sta;
}
