void __ieee80211_tx_skb_tid_band(struct ieee80211_sub_if_data *sdata,
				 struct sk_buff *skb, int tid,
				 enum ieee80211_band band)
{
	int ac = ieee802_1d_to_ac[tid & 7];

	skb_set_mac_header(skb, 0);
	skb_set_network_header(skb, 0);
	skb_set_transport_header(skb, 0);

	skb_set_queue_mapping(skb, ac);
	skb->priority = tid;

	skb->dev = sdata->dev;

	/*
	 * The other path calling ieee80211_xmit is from the tasklet,
	 * and while we can handle concurrent transmissions locking
	 * requirements are that we do not come into tx with bhs on.
	 */
	local_bh_disable();
	ieee80211_xmit(sdata, skb, band);
	local_bh_enable();
}
