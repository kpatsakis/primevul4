ieee80211_tx_h_dynamic_ps(struct ieee80211_tx_data *tx)
{
	struct ieee80211_local *local = tx->local;
	struct ieee80211_if_managed *ifmgd;

	/* driver doesn't support power save */
	if (!(local->hw.flags & IEEE80211_HW_SUPPORTS_PS))
		return TX_CONTINUE;

	/* hardware does dynamic power save */
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_DYNAMIC_PS)
		return TX_CONTINUE;

	/* dynamic power save disabled */
	if (local->hw.conf.dynamic_ps_timeout <= 0)
		return TX_CONTINUE;

	/* we are scanning, don't enable power save */
	if (local->scanning)
		return TX_CONTINUE;

	if (!local->ps_sdata)
		return TX_CONTINUE;

	/* No point if we're going to suspend */
	if (local->quiescing)
		return TX_CONTINUE;

	/* dynamic ps is supported only in managed mode */
	if (tx->sdata->vif.type != NL80211_IFTYPE_STATION)
		return TX_CONTINUE;

	ifmgd = &tx->sdata->u.mgd;

	/*
	 * Don't wakeup from power save if u-apsd is enabled, voip ac has
	 * u-apsd enabled and the frame is in voip class. This effectively
	 * means that even if all access categories have u-apsd enabled, in
	 * practise u-apsd is only used with the voip ac. This is a
	 * workaround for the case when received voip class packets do not
	 * have correct qos tag for some reason, due the network or the
	 * peer application.
	 *
	 * Note: ifmgd->uapsd_queues access is racy here. If the value is
	 * changed via debugfs, user needs to reassociate manually to have
	 * everything in sync.
	 */
	if ((ifmgd->flags & IEEE80211_STA_UAPSD_ENABLED) &&
	    (ifmgd->uapsd_queues & IEEE80211_WMM_IE_STA_QOSINFO_AC_VO) &&
	    skb_get_queue_mapping(tx->skb) == IEEE80211_AC_VO)
		return TX_CONTINUE;

	if (local->hw.conf.flags & IEEE80211_CONF_PS) {
		ieee80211_stop_queues_by_reason(&local->hw,
						IEEE80211_MAX_QUEUE_MAP,
						IEEE80211_QUEUE_STOP_REASON_PS);
		ifmgd->flags &= ~IEEE80211_STA_NULLFUNC_ACKED;
		ieee80211_queue_work(&local->hw,
				     &local->dynamic_ps_disable_work);
	}

	/* Don't restart the timer if we're not disassociated */
	if (!ifmgd->associated)
		return TX_CONTINUE;

	mod_timer(&local->dynamic_ps_timer, jiffies +
		  msecs_to_jiffies(local->hw.conf.dynamic_ps_timeout));

	return TX_CONTINUE;
}
