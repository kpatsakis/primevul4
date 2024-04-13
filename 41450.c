static int tg3_poll(struct napi_struct *napi, int budget)
{
	struct tg3_napi *tnapi = container_of(napi, struct tg3_napi, napi);
	struct tg3 *tp = tnapi->tp;
	int work_done = 0;
	struct tg3_hw_status *sblk = tnapi->hw_status;

	while (1) {
		if (sblk->status & SD_STATUS_ERROR)
			tg3_process_error(tp);

		tg3_poll_link(tp);

		work_done = tg3_poll_work(tnapi, work_done, budget);

		if (unlikely(tg3_flag(tp, TX_RECOVERY_PENDING)))
			goto tx_recovery;

		if (unlikely(work_done >= budget))
			break;

		if (tg3_flag(tp, TAGGED_STATUS)) {
			/* tp->last_tag is used in tg3_int_reenable() below
			 * to tell the hw how much work has been processed,
			 * so we must read it before checking for more work.
			 */
			tnapi->last_tag = sblk->status_tag;
			tnapi->last_irq_tag = tnapi->last_tag;
			rmb();
		} else
			sblk->status &= ~SD_STATUS_UPDATED;

		if (likely(!tg3_has_work(tnapi))) {
			napi_complete(napi);
			tg3_int_reenable(tnapi);
			break;
		}
	}

	return work_done;

tx_recovery:
	/* work_done is guaranteed to be less than budget. */
	napi_complete(napi);
	tg3_reset_task_schedule(tp);
	return work_done;
}
