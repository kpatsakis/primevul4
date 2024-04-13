static void tg3_reset_task(struct work_struct *work)
{
	struct tg3 *tp = container_of(work, struct tg3, reset_task);
	int err;

	tg3_full_lock(tp, 0);

	if (!netif_running(tp->dev)) {
		tg3_flag_clear(tp, RESET_TASK_PENDING);
		tg3_full_unlock(tp);
		return;
	}

	tg3_full_unlock(tp);

	tg3_phy_stop(tp);

	tg3_netif_stop(tp);

	tg3_full_lock(tp, 1);

	if (tg3_flag(tp, TX_RECOVERY_PENDING)) {
		tp->write32_tx_mbox = tg3_write32_tx_mbox;
		tp->write32_rx_mbox = tg3_write_flush_reg32;
		tg3_flag_set(tp, MBOX_WRITE_REORDER);
		tg3_flag_clear(tp, TX_RECOVERY_PENDING);
	}

	tg3_halt(tp, RESET_KIND_SHUTDOWN, 0);
	err = tg3_init_hw(tp, 1);
	if (err)
		goto out;

	tg3_netif_start(tp);

out:
	tg3_full_unlock(tp);

	if (!err)
		tg3_phy_start(tp);

	tg3_flag_clear(tp, RESET_TASK_PENDING);
}
