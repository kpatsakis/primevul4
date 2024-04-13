static int invoke_tx_handlers(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	ieee80211_tx_result res = TX_DROP;

#define CALL_TXH(txh) \
	do {				\
		res = txh(tx);		\
		if (res != TX_CONTINUE)	\
			goto txh_done;	\
	} while (0)

	CALL_TXH(ieee80211_tx_h_dynamic_ps);
	CALL_TXH(ieee80211_tx_h_check_assoc);
	CALL_TXH(ieee80211_tx_h_ps_buf);
	CALL_TXH(ieee80211_tx_h_check_control_port_protocol);
	CALL_TXH(ieee80211_tx_h_select_key);
	if (!(tx->local->hw.flags & IEEE80211_HW_HAS_RATE_CONTROL))
		CALL_TXH(ieee80211_tx_h_rate_ctrl);

	if (unlikely(info->flags & IEEE80211_TX_INTFL_RETRANSMISSION)) {
		__skb_queue_tail(&tx->skbs, tx->skb);
		tx->skb = NULL;
		goto txh_done;
	}

	CALL_TXH(ieee80211_tx_h_michael_mic_add);
	CALL_TXH(ieee80211_tx_h_sequence);
	CALL_TXH(ieee80211_tx_h_fragment);
	/* handlers after fragment must be aware of tx info fragmentation! */
	CALL_TXH(ieee80211_tx_h_stats);
	CALL_TXH(ieee80211_tx_h_encrypt);
	if (!(tx->local->hw.flags & IEEE80211_HW_HAS_RATE_CONTROL))
		CALL_TXH(ieee80211_tx_h_calculate_duration);
#undef CALL_TXH

 txh_done:
	if (unlikely(res == TX_DROP)) {
		I802_DEBUG_INC(tx->local->tx_handlers_drop);
		if (tx->skb)
			ieee80211_free_txskb(&tx->local->hw, tx->skb);
		else
			ieee80211_purge_tx_queue(&tx->local->hw, &tx->skbs);
		return -1;
	} else if (unlikely(res == TX_QUEUED)) {
		I802_DEBUG_INC(tx->local->tx_handlers_queued);
		return -1;
	}

	return 0;
}
