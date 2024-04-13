static bool ieee80211_tx_frags(struct ieee80211_local *local,
			       struct ieee80211_vif *vif,
			       struct ieee80211_sta *sta,
			       struct sk_buff_head *skbs,
			       bool txpending)
{
	struct ieee80211_tx_control control;
	struct sk_buff *skb, *tmp;
	unsigned long flags;

	skb_queue_walk_safe(skbs, skb, tmp) {
		struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
		int q = info->hw_queue;

#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
		if (WARN_ON_ONCE(q >= local->hw.queues)) {
			__skb_unlink(skb, skbs);
			ieee80211_free_txskb(&local->hw, skb);
			continue;
		}
#endif

		spin_lock_irqsave(&local->queue_stop_reason_lock, flags);
		if (local->queue_stop_reasons[q] ||
		    (!txpending && !skb_queue_empty(&local->pending[q]))) {
			if (unlikely(info->flags &
				     IEEE80211_TX_INTFL_OFFCHAN_TX_OK)) {
				if (local->queue_stop_reasons[q] &
				    ~BIT(IEEE80211_QUEUE_STOP_REASON_OFFCHANNEL)) {
					/*
					 * Drop off-channel frames if queues
					 * are stopped for any reason other
					 * than off-channel operation. Never
					 * queue them.
					 */
					spin_unlock_irqrestore(
						&local->queue_stop_reason_lock,
						flags);
					ieee80211_purge_tx_queue(&local->hw,
								 skbs);
					return true;
				}
			} else {

				/*
				 * Since queue is stopped, queue up frames for
				 * later transmission from the tx-pending
				 * tasklet when the queue is woken again.
				 */
				if (txpending)
					skb_queue_splice_init(skbs,
							      &local->pending[q]);
				else
					skb_queue_splice_tail_init(skbs,
								   &local->pending[q]);

				spin_unlock_irqrestore(&local->queue_stop_reason_lock,
						       flags);
				return false;
			}
		}
		spin_unlock_irqrestore(&local->queue_stop_reason_lock, flags);

		info->control.vif = vif;
		control.sta = sta;

		__skb_unlink(skb, skbs);
		drv_tx(local, &control, skb);
	}

	return true;
}
