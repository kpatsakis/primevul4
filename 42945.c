void ieee80211_csa_finish(struct ieee80211_vif *vif)
{
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);

	ieee80211_queue_work(&sdata->local->hw,
			     &sdata->csa_finalize_work);
}
