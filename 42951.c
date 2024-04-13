struct sk_buff *ieee80211_proberesp_get(struct ieee80211_hw *hw,
					struct ieee80211_vif *vif)
{
	struct ieee80211_if_ap *ap = NULL;
	struct sk_buff *skb = NULL;
	struct probe_resp *presp = NULL;
	struct ieee80211_hdr *hdr;
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);

	if (sdata->vif.type != NL80211_IFTYPE_AP)
		return NULL;

	rcu_read_lock();

	ap = &sdata->u.ap;
	presp = rcu_dereference(ap->probe_resp);
	if (!presp)
		goto out;

	skb = dev_alloc_skb(presp->len);
	if (!skb)
		goto out;

	memcpy(skb_put(skb, presp->len), presp->data, presp->len);

	hdr = (struct ieee80211_hdr *) skb->data;
	memset(hdr->addr1, 0, sizeof(hdr->addr1));

out:
	rcu_read_unlock();
	return skb;
}
