static inline void tg3_netif_start(struct tg3 *tp)
{
	tg3_ptp_resume(tp);

	/* NOTE: unconditional netif_tx_wake_all_queues is only
	 * appropriate so long as all callers are assured to
	 * have free tx slots (such as after tg3_init_hw)
	 */
	netif_tx_wake_all_queues(tp->dev);

	if (tp->link_up)
		netif_carrier_on(tp->dev);

	tg3_napi_enable(tp);
	tp->napi[0].hw_status->status |= SD_STATUS_UPDATED;
	tg3_enable_ints(tp);
}
