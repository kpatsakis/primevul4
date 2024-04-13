bool ieee80211_csa_is_complete(struct ieee80211_vif *vif)
{
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);
	struct beacon_data *beacon = NULL;
	u8 *beacon_data;
	size_t beacon_data_len;
	int counter_beacon = sdata->csa_counter_offset_beacon;
	int ret = false;

	if (!ieee80211_sdata_running(sdata))
		return false;

	rcu_read_lock();
	if (vif->type == NL80211_IFTYPE_AP) {
		struct ieee80211_if_ap *ap = &sdata->u.ap;

		beacon = rcu_dereference(ap->beacon);
		if (WARN_ON(!beacon || !beacon->tail))
			goto out;
		beacon_data = beacon->tail;
		beacon_data_len = beacon->tail_len;
	} else if (vif->type == NL80211_IFTYPE_ADHOC) {
		struct ieee80211_if_ibss *ifibss = &sdata->u.ibss;

		beacon = rcu_dereference(ifibss->presp);
		if (!beacon)
			goto out;

		beacon_data = beacon->head;
		beacon_data_len = beacon->head_len;
	} else if (vif->type == NL80211_IFTYPE_MESH_POINT) {
		struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;

		beacon = rcu_dereference(ifmsh->beacon);
		if (!beacon)
			goto out;

		beacon_data = beacon->head;
		beacon_data_len = beacon->head_len;
	} else {
		WARN_ON(1);
		goto out;
	}

	if (WARN_ON(counter_beacon > beacon_data_len))
		goto out;

	if (beacon_data[counter_beacon] == 0)
		ret = true;
 out:
	rcu_read_unlock();

	return ret;
}
